#include <xt/api/private/Platform.hpp>
#include <xt/private/BlockingStream.hpp>
#include <xt/private/Shared.hpp>
#include <xt/Private.hpp>
#include <thread>

XtBlockingStream::
~XtBlockingStream() 
{ if(!_secondary) SendControl(XtBlockingStreamState::Closing, XtBlockingStreamState::Closed); }

XtBlockingStream::
XtBlockingStream(bool secondary):
_index(-1),
_aggregated(false),
_lock(),
_secondary(secondary),
_state(XtBlockingStreamState::Stopped),
_control(),
_respond()
{
  if(secondary) return;
  std::thread t(OnBlockingBuffer, this);
  t.detach();
}

void
XtBlockingStream::RequestStop()
{
  StopStream();
  if(_secondary) return;
  std::unique_lock guard(_lock);
  _state = XtBlockingStreamState::Stopped;
  guard.unlock();
  _respond.notify_one();
}

XtBlockingStreamState
XtBlockingStream::ReadState()
{
  std::lock_guard<std::mutex> guard(_lock);
  return _state;
}

void
XtBlockingStream::ReceiveControl(XtBlockingStreamState state)
{
  std::unique_lock guard(_lock);
  _state = state;
  guard.unlock();
  _respond.notify_one();
}

void 
XtBlockingStream::OnXRun() const
{
  auto xRun = _params.stream.onXRun;
  if(xRun == nullptr) return;
  if(!_aggregated) xRun(-1, _user);
  else xRun(_index, static_cast<XtAggregateContext*>(_user)->stream->_user);
}

void
XtBlockingStream::SendControl(XtBlockingStreamState from, XtBlockingStreamState to)
{
  std::unique_lock guard(_lock);
  if(_state == to) return;
  _state = from;
  _control.notify_one();
  auto pred = [this, to] { return _state == to; };
  auto timeout = std::chrono::microseconds(WaitTimeoutMs);
  XT_ASSERT(_respond.wait_for(guard, timeout, pred));
}

XtFault
XtBlockingStream::Start() 
{
  if(_secondary) { ProcessBuffer(true); StartStream(); }
  else SendControl(XtBlockingStreamState::Starting, XtBlockingStreamState::Started);
  return 0;
}

XtFault
XtBlockingStream::Stop() 
{
  if(_secondary) StopStream();
  else SendControl(XtBlockingStreamState::Stopping, XtBlockingStreamState::Stopped);
  return 0;
}

bool
XtBlockingStream::VerifyOnBuffer(XtLocation const& location, XtFault fault, char const* expr)
{
  if(fault == 0) return true;
  XtBuffer buffer = { 0 };
  RequestStop();
  XtiTrace(location, expr);
  buffer.error = XtiCreateError(GetSystem(), fault);
  OnBuffer(&buffer);
  return false;
}

void
XtBlockingStream::OnBlockingBuffer(XtBlockingStream* stream)
{  
  int32_t threadPolicy;
  int32_t prevThreadPrio;
  XtBlockingStreamState state;
  XtPlatform::BeginThread();
  XtPlatform::RaiseThreadPriority(&threadPolicy, &prevThreadPrio);
  while((state = stream->ReadState()) != XtBlockingStreamState::Closed)
    switch(state)
    {
    case XtBlockingStreamState::Started:
      stream->ProcessBuffer(false);
      break;
    case XtBlockingStreamState::Closing:
      stream->ReceiveControl(XtBlockingStreamState::Closed);
      break;
    case XtBlockingStreamState::Stopping:
      stream->StopStream();
      stream->ReceiveControl(XtBlockingStreamState::Stopped);
      break;
    case XtBlockingStreamState::Starting:
      stream->ProcessBuffer(true);
      stream->StartStream();
      stream->ReceiveControl(XtBlockingStreamState::Started);
      break;
    case XtBlockingStreamState::Stopped:
      {
      auto timeout = std::chrono::milliseconds(WaitTimeoutMs);
      auto pred = [stream] { return stream->_state != XtBlockingStreamState::Stopped; };
      std::unique_lock guard(stream->_lock);
      XT_ASSERT(stream->_control.wait_for(guard, timeout, pred));
      break;
      }
    default:
      XT_ASSERT(false);
      break;
    }  
  XtPlatform::RevertThreadPriority(threadPolicy, prevThreadPrio);
  XtPlatform::EndThread();
}