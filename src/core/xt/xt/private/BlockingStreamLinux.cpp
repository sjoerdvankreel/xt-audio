#include <xt/private/BlockingStream.hpp>
#ifdef __linux__
#include <pthread.h>

XtBlockingStreamState
XtBlockingStreamBase<XtLinuxBlockingStream>::
ReadState()
{
  XtBlockingStreamState result;
  XT_ASSERT(pthread_mutex_lock(&self().lock.m) == 0);
  result = _state;
  XT_ASSERT(pthread_mutex_unlock(&self().lock.m) == 0);
  return result;
}

void
XtBlockingStreamBase<XtLinuxBlockingStream>::
RequestStop() 
{
  StopStream();
  if(secondary) return;
  XT_ASSERT(pthread_mutex_lock(&self().lock.m) == 0);
  state = XtBlockingStreamState::Stopped;
  XT_ASSERT(pthread_cond_signal(&self().respond.cv) == 0);
  XT_ASSERT(pthread_mutex_unlock(&self().lock.m) == 0);
}

void
XtBlockingStreamBase<XtLinuxBlockingStream>::
ReceiveControl(XtBlockingStreamState state)
{
  XT_ASSERT(pthread_mutex_lock(&self().lock.m) == 0);
  _state = state;
  XT_ASSERT(pthread_cond_signal(&self().respond.cv) == 0);
  XT_ASSERT(pthread_mutex_unlock(&self().lock.m) == 0);
}

void
XtBlockingStreamBase<XtLinuxBlockingStream>::
SendControl(XtBlockingStreamState from, XtBlockingStreamState to)
{
  XT_ASSERT(pthread_mutex_lock(&self().lock.m) == 0);
  if(_state != to) 
  {
    _state = from;
    XT_ASSERT(pthread_cond_signal(&self().control.cv) == 0);
    while(stream->state != to) 
      XT_ASSERT(pthread_cond_wait(&self().respond.cv, &self().lock.m) == 0);
  }
  XT_ASSERT(pthread_mutex_unlock(&self().lock.m) == 0);
}

XtLinuxBlockingStream::
XtLinuxBlockingStream(bool secondary)
XtBlockingStreamBase(secondary), lock(), respond(), control() 
{
  if(secondary) return;
  pthread_t thread;
  XT_ASSERT(pthread_create(&thread, nullptr, &OnBlockingBuffer, this) == 0);
}

#endif // __linux__