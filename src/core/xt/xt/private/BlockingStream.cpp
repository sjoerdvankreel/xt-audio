#include <xt/api/private/Platform.hpp>
#include <xt/private/BlockingStream.hpp>
#include <xt/private/AggregateStream.hpp>
#include <xt/private/Shared.hpp>
#include <thread>

XtBool
XtBlockingStream::IsRunning() const
{ return _state.load() == State::Started; }

XtBlockingStream::
XtBlockingStream(bool secondary):
_index(-1), _aggregated(false), _received(false), _lock(),
_secondary(secondary), _state(State::Stopped), _control(), _respond()
{
  if(secondary) return;
  std::thread t(RunBlockingStream, this);
  t.detach();
}

XtBlockingStream::
~XtBlockingStream() 
{ 
  if(_secondary) return;
  SendControl(State::Closing); 
}

XtFault
XtBlockingStream::Start() 
{
  XT_ASSERT(!_secondary);
  SendControl(State::Starting);
  return 0;
}

XtFault
XtBlockingStream::Stop() 
{
  XT_ASSERT(!_secondary);
  SendControl(State::Stopping);
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
XtBlockingStream::SendControl(State from)
{
  std::unique_lock guard(_lock);
  _state = from;
  _received = false;
  _control.notify_one();
  auto pred = [this] { return _received; };
  auto timeout = std::chrono::milliseconds(WaitTimeoutMs);
  XT_ASSERT(_respond.wait_for(guard, timeout, pred));
}

void
XtBlockingStream::ReceiveControl(State state)
{
  std::unique_lock guard(_lock);
  _state = state;
  _received = true;
  guard.unlock();
  _respond.notify_one();  
  if(state == State::Started) OnRunning(XtTrue);
  if(state == State::Stopped) OnRunning(XtFalse);
}   

void
XtBlockingStream::RunBlockingStream(XtBlockingStream* stream)
{  
  State state;
  int32_t threadPolicy;
  int32_t prevThreadPrio;
  XtPlatform::BeginThread();
  XtPlatform::RaiseThreadPriority(&threadPolicy, &prevThreadPrio);
  while((state = stream->_state.load()) != State::Closed)
    switch(state)
    {
    case State::Closing:
      stream->ReceiveControl(State::Closed);
      break;
    case State::Stopping:
      stream->StopStream();
      stream->ReceiveControl(State::Stopped);
      break;
    case State::Started:
      if(stream->ProcessBuffer(false) != 0)
      {
        stream->StopStream();
        stream->ReceiveControl(State::Stopped);
      }
      break;
    case State::Starting:
      if(stream->ProcessBuffer(true) != 0 || stream->StartStream() != 0)
        stream->ReceiveControl(State::Stopped);
      else
        stream->ReceiveControl(State::Started);
      break;
    case State::Stopped:
      {
      auto pred = [stream] { return stream->_state != State::Stopped; };
      std::unique_lock guard(stream->_lock);
      stream->_control.wait(guard, pred);
      break;
      }
    default:
      XT_ASSERT(false);
      break;
    }  
  XtPlatform::RevertThreadPriority(threadPolicy, prevThreadPrio);
  XtPlatform::EndThread();
}