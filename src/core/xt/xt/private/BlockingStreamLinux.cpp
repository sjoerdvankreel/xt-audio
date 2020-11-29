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

XtBlockingStreamState
XtBlockingStream::ReadState()
{
  XtBlockingStreamState result;
  XT_ASSERT(pthread_mutex_lock(&stream->lock.m) == 0);
  result = stream->state;
  XT_ASSERT(pthread_mutex_unlock(&stream->lock.m) == 0);
  return result;
}

void
XtBlockingStream::ReceiveControl(XtBlockingStreamState state)
{
  XT_ASSERT(pthread_mutex_lock(&_impl.lock.m) == 0);
  stream->state = state;
  XT_ASSERT(pthread_cond_signal(&_impl.respond.cv) == 0);
  XT_ASSERT(pthread_mutex_unlock(&_impl.lock.m) == 0);
}

XtBlockingStreamImpl::
XtBlockingStreamImpl(bool secondary)
lock(), respond(), control() 
{
  if(secondary) return;
  pthread_t thread;
  XT_ASSERT(pthread_create(&thread, nullptr, &LinuxOnBlockingBuffer, this) == 0);
}

void
XtBlockingStream::SendControl(XtBlockingStreamState from, XtBlockingStreamState to)
{
  XT_ASSERT(pthread_mutex_lock(&_impl.lock.m) == 0);
  if(stream->state != to) 
  {
    stream->state = from;
    XT_ASSERT(pthread_cond_signal(&_impl.control.cv) == 0);
    while(stream->state != to) 
      XT_ASSERT(pthread_cond_wait(&_impl.respond.cv, &_impl.lock.m) == 0);
  }
  XT_ASSERT(pthread_mutex_unlock(&_impl.lock.m) == 0);
}

#endif // __linux__