#include <xt/private/BlockingStream.hpp>
#ifdef _WIN32

XtBlockingStreamState
XtBlockingStream::ReadState()
{
  XtBlockingStreamState result;
  EnterCriticalSection(&_impl.lock.cs);
  result = _state;
  LeaveCriticalSection(&_impl.lock.cs);
  return result;
}

void
XtBlockingStream::RequestStop() 
{
  StopStream();
  if(_secondary) return;
  EnterCriticalSection(&_impl.lock.cs);
  _state = XtBlockingStreamState::Stopped;
  XT_ASSERT(SetEvent(_impl.respond.event));
  LeaveCriticalSection(&_impl.lock.cs);
}

void
XtBlockingStream::ReceiveControl(XtBlockingStreamState state)
{
  EnterCriticalSection(&_impl.lock.cs);
  _state = state;
  XT_ASSERT(SetEvent(_impl.respond.event));
  LeaveCriticalSection(&_impl.lock.cs);
}

XtBlockingStreamImpl::
XtBlockingStreamImpl(bool secondary):
respond(), control(), lock()
{
  if(secondary) return;
  HANDLE thread = CreateThread(nullptr, 0, &OnWin32BlockingBuffer, this, 0, nullptr);
  XT_ASSERT(thread != nullptr);
  CloseHandle(thread);
}

void
XtBlockingStream::SendControl(XtBlockingStreamState from, XtBlockingStreamState to)
{
  EnterCriticalSection(&_impl.lock.cs);
  if(_state == to) { LeaveCriticalSection(&_impl.lock.cs); return; }
  _state = from;
  XT_ASSERT(SetEvent(_impl.control.event));
  LeaveCriticalSection(&_impl.lock.cs);
  while(ReadState() != to) 
    XT_ASSERT(WaitForSingleObject(_impl.respond.event, XT_WAIT_TIMEOUT_MS) == WAIT_OBJECT_0);
}

#endif // _WIN32