#include <xt/private/BlockingStream.hpp>
#ifdef _WIN32

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

XtBlockingStreamImpl::
XtBlockingStreamImpl(bool secondary):
respond(), control(), lock()
{
  if(secondary) return;
  HANDLE thread = CreateThread(nullptr, 0, &OnWin32BlockingBuffer, this, 0, nullptr);
  XT_ASSERT(thread != nullptr);
  CloseHandle(thread);
}

#endif // _WIN32