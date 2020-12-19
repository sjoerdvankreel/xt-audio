#include <xt/shared/Shared.hpp>
#include <xt/private/Platform.hpp>
#include <xt/blocking/Adapter.hpp>
#include <thread>

XtBlockingAdapter::
~XtBlockingAdapter() 
{ SendControl(State::Closing); }
void
XtBlockingAdapter::Stop()
{ SendControl(State::Stopping); }
XtSystem
XtBlockingAdapter::GetSystem() const
{ return _stream->GetSystem(); }
XtFault
XtBlockingAdapter::Start() 
{ SendControl(State::Starting); return 0; }
XtBool
XtBlockingAdapter::IsRunning() const
{ return _state.load() == State::Started; }
XtFault
XtBlockingAdapter::GetFrames(int32_t* frames) const
{ return _stream->GetFrames(frames); }
XtFault
XtBlockingAdapter::GetLatency(XtLatency* latency) const
{ return _stream->GetLatency(latency); }

XtBlockingAdapter::
XtBlockingAdapter(XtBlockingStream* stream):
_received(false), _lock(), _state(State::Stopped), 
_control(), _respond(), _stream(stream)
{
  stream->_runner = this;
  std::thread t(RunBlockingStream, this);
  t.detach();
}

void
XtBlockingAdapter::ReceiveControl(State state)
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
XtBlockingAdapter::SendControl(State from)
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
XtBlockingAdapter::RunBlockingStream(XtBlockingAdapter* adapter)
{  
  State state;
  int32_t threadPolicy;
  int32_t prevThreadPrio;
  XtPlatform::BeginThread();
  XtPlatform::RaiseThreadPriority(&threadPolicy, &prevThreadPrio);
  while((state = adapter->_state.load()) != State::Closed)
    switch(state)
    {
    case State::Closing:
      adapter->ReceiveControl(State::Closed);
      break;
    case State::Stopping:
      adapter->_stream->StopBuffer();
      adapter->ReceiveControl(State::Stopped);
      break;
    case State::Started:      
      if(adapter->_stream->BlockMasterBuffer() != 0 || adapter->_stream->ProcessBuffer() != 0)
      {
        adapter->_stream->StopBuffer();
        adapter->ReceiveControl(State::Stopped);
      }
      break;
    case State::Starting:
      if(adapter->_stream->PrefillOutputBuffer() != 0 || adapter->_stream->StartBuffer() != 0)
        adapter->ReceiveControl(State::Stopped);
      else
        adapter->ReceiveControl(State::Started);
      break;
    case State::Stopped:
      {
      auto pred = [adapter] { return adapter->_state != State::Stopped; };
      std::unique_lock guard(adapter->_lock);
      adapter->_control.wait(guard, pred);
      break;
      }
    default:
      XT_ASSERT(false);
      break;
    }  
  XtPlatform::RevertThreadPriority(threadPolicy, prevThreadPrio);
  XtPlatform::EndThread();
}