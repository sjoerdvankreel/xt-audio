#include <xt/shared/Shared.hpp>
#include <xt/private/Platform.hpp>
#include <xt/blocking/Runner.hpp>
#include <thread>

XtBlockingRunner::
~XtBlockingRunner() 
{ SendControl(State::Closing); }
void
XtBlockingRunner::Stop()
{ SendControl(State::Stopping); }
XtSystem
XtBlockingRunner::GetSystem() const
{ return _stream->GetSystem(); }
void*
XtBlockingRunner::GetHandle() const
{ return _stream->GetHandle(); }
XtFault
XtBlockingRunner::Start() 
{ SendControl(State::Starting); return 0; }
XtBool
XtBlockingRunner::IsRunning() const
{ return _state.load() == State::Started; }
XtFault
XtBlockingRunner::GetFrames(int32_t* frames) const
{ return _stream->GetFrames(frames); }
XtFault
XtBlockingRunner::GetLatency(XtLatency* latency) const
{ return _stream->GetLatency(latency); }

XtBlockingRunner::
XtBlockingRunner(XtBlockingStream* stream):
_received(false), _lock(), _state(State::Stopped), 
_control(), _respond(), _stream(stream)
{
  stream->_runner = this;
  std::thread t(RunBlockingStream, this);
  t.detach();
}

void
XtBlockingRunner::ReceiveControl(State state, XtFault fault)
{
  std::unique_lock guard(_lock);
  _state = state;
  _received = true;
  guard.unlock();
  _respond.notify_one();  
  if(state == State::Started) OnRunning(XtTrue, 0);
  if(state == State::Stopped) OnRunning(XtFalse, fault);
}   

void
XtBlockingRunner::SendControl(State from)
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
XtBlockingRunner::RunBlockingStream(XtBlockingRunner* runner)
{  
  State state;
  XtBool ready;
  XtFault fault;
  int32_t threadPolicy;
  int32_t prevThreadPrio;
  XtPlatform::BeginThread();
  XtPlatform::RaiseThreadPriority(&threadPolicy, &prevThreadPrio);

  while((state = runner->_state.load()) != State::Closed)
    switch(state)
    {
    case State::Closing:
      runner->ReceiveControl(State::Closed, 0);
      break;
    case State::Stopping:
      runner->_stream->StopBuffer();
      runner->ReceiveControl(State::Stopped, 0);
      break;
    case State::Started:   
      fault = 0;   
      ready = XtFalse;
      while(!ready && fault == 0)
        fault = runner->_stream->BlockMasterBuffer(&ready);
      if(ready && fault == 0)
        fault = runner->_stream->ProcessBuffer();
      if(fault != 0)
      {
        runner->_stream->StopBuffer();
        runner->ReceiveControl(State::Stopped, fault);
      }
      break;
    case State::Starting:
      if(((fault = runner->_stream->PrefillOutputBuffer()) != 0) ||
         ((fault = runner->_stream->StartBuffer()) != 0))
        runner->ReceiveControl(State::Stopped, fault);
      else
        runner->ReceiveControl(State::Started, 0);
      break;
    case State::Stopped:
      {
      auto pred = [runner] { return runner->_state != State::Stopped; };
      std::unique_lock guard(runner->_lock);
      runner->_control.wait(guard, pred);
      break;
      }
    default:
      XT_ASSERT(false);
      break;
    }  
  XtPlatform::RevertThreadPriority(threadPolicy, prevThreadPrio);
  XtPlatform::EndThread();
}