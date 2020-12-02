#include <xt/api/private/Platform.hpp>
#include <xt/private/BlockingStream.hpp>
#include <xt/private/Shared.hpp>
#include <xt/Private.hpp>
#include <thread>

XtBlockingStream::
XtBlockingStream(bool secondary):
_index(-1), _aggregated(false), _lock(), _secondary(secondary),
_state(State::Stopped), _control(), _respond()
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
  _state = State::Stopped;
  guard.unlock();
  _respond.notify_one();
}

XtBlockingStream::
~XtBlockingStream() 
{ 
  if(_secondary) return;
  SendControl(State::Closing, State::Closed); 
}

void
XtBlockingStream::ReceiveControl(State state)
{
  std::unique_lock guard(_lock);
  _state = state;
  guard.unlock();
  _respond.notify_one();
}

XtFault
XtBlockingStream::Start() 
{
  if(_secondary)
  {
    ProcessBuffer(true);
    StartStream();
    return 0;
  }
  SendControl(State::Starting, State::Started);
  return 0;
}

XtFault
XtBlockingStream::Stop() 
{
  if(_secondary) StopStream();
  else SendControl(State::Stopping, State::Stopped);
  return 0;
}

void 
XtBlockingStream::OnXRun() const
{
  auto xRun = _params.stream.onXRun;
  if(xRun == nullptr) return;
  if(!_aggregated) return xRun(-1, _user), void();
  auto context = static_cast<XtAggregateContext*>(_user);
  xRun(_index, context->stream->_user);
}

void
XtBlockingStream::SendControl(State from, State to)
{
  std::unique_lock guard(_lock);
  if(_state == to) return;
  _state = from;
  _control.notify_one();
  auto pred = [this, to] { return _state == to; };
  auto timeout = std::chrono::milliseconds(WaitTimeoutMs);
  XT_ASSERT(_respond.wait_for(guard, timeout, pred));
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
  State state;
  int32_t threadPolicy;
  int32_t prevThreadPrio;
  XtPlatform::BeginThread();
  XtPlatform::RaiseThreadPriority(&threadPolicy, &prevThreadPrio);
  while((state = stream->_state.load()) != State::Closed)
    switch(state)
    {
    case State::Started:
      stream->ProcessBuffer(false);
      break;
    case State::Closing:
      stream->ReceiveControl(State::Closed);
      break;
    case State::Stopping:
      stream->StopStream();
      stream->ReceiveControl(State::Stopped);
      break;
    case State::Starting:
      stream->ProcessBuffer(true);
      stream->StartStream();
      stream->ReceiveControl(State::Started);
      break;
    case State::Stopped:
      {
      auto timeout = std::chrono::milliseconds(WaitTimeoutMs);
      auto pred = [stream] { return stream->_state != State::Stopped; };
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