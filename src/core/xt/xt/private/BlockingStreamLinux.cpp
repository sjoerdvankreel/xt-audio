#include <xt/private/BlockingStream.hpp>
#ifdef __linux__

void
XtBlockingStream::RequestStop() 
{
  StopStream();
  if(secondary) return;
  XT_ASSERT(pthread_mutex_lock(&lock.m) == 0);
  state = XtBlockingStreamState::Stopped;
  XT_ASSERT(pthread_cond_signal(&respondCv.cv) == 0);
  XT_ASSERT(pthread_mutex_unlock(&lock.m) == 0);
}

XtBlockingStreamImpl::
XtBlockingStreamImpl(bool secondary)
lock(), respond(), control() 
{
  if(secondary) return;
  pthread_t thread;
  XT_ASSERT(pthread_create(&thread, nullptr, &LinuxOnBlockingBuffer, this) == 0);
}

#endif // __linux__