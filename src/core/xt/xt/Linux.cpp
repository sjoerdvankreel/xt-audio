#ifdef __linux__
#include <xt/Linux.hpp>
#include <pthread.h>

// ---- local ----

static XtBlockingStreamState ReadLinuxBlockingStreamState(
  XtlLinuxBlockingStream* stream) {

  XtBlockingStreamState result;
  XT_ASSERT(pthread_mutex_lock(&stream->lock.m) == 0);
  result = stream->state;
  XT_ASSERT(pthread_mutex_unlock(&stream->lock.m) == 0);
  return result;
}

static void ReceiveLinuxBlockingStreamControl(
  XtlLinuxBlockingStream* stream, XtBlockingStreamState state) {

  XT_ASSERT(pthread_mutex_lock(&stream->lock.m) == 0);
  stream->state = state;
  XT_ASSERT(pthread_cond_signal(&stream->respondCv.cv) == 0);
  XT_ASSERT(pthread_mutex_unlock(&stream->lock.m) == 0);
}

static void SendLinuxBlockingStreamControl(
  XtlLinuxBlockingStream* stream, XtBlockingStreamState from, XtBlockingStreamState to) {

  XT_ASSERT(pthread_mutex_lock(&stream->lock.m) == 0);
  if(stream->state == to) {
    XT_ASSERT(pthread_mutex_unlock(&stream->lock.m) == 0);
    return;
  }
  stream->state = from;
  XT_ASSERT(pthread_cond_signal(&stream->controlCv.cv) == 0);
  while(stream->state != to)
    XT_ASSERT(pthread_cond_wait(&stream->respondCv.cv, &stream->lock.m) == 0);
  XT_ASSERT(pthread_mutex_unlock(&stream->lock.m) == 0);
}

static void* LinuxOnBlockingBuffer(void* user) {

  int policy;
  int maxPriority;
  int startPriority;
  struct sched_param param;
  XtBlockingStreamState state;
  auto stream = static_cast<XtlLinuxBlockingStream*>(user);

  XT_ASSERT(pthread_getschedparam(pthread_self(), &policy, &param) == 0);
  startPriority = param.sched_priority;
  maxPriority = sched_get_priority_max(policy);
  while((state = ReadLinuxBlockingStreamState(stream)) != XtBlockingStreamState::Closed) {
    switch(state) {
    case XtBlockingStreamState::Started:
      stream->ProcessBuffer(false);
      break;
    case XtBlockingStreamState::Closing:
      ReceiveLinuxBlockingStreamControl(stream, XtBlockingStreamState::Closed);
      return nullptr;
    case XtBlockingStreamState::Stopping:
      stream->StopStream();
      param.sched_priority = startPriority;
      XT_ASSERT(pthread_setschedparam(pthread_self(), policy, &param) == 0);
      ReceiveLinuxBlockingStreamControl(stream, XtBlockingStreamState::Stopped);
      break;
    case XtBlockingStreamState::Starting:
      stream->ProcessBuffer(true);
      param.sched_priority = maxPriority;
      XT_ASSERT(pthread_setschedparam(pthread_self(), policy, &param) == 0);
      stream->StartStream();
      ReceiveLinuxBlockingStreamControl(stream, XtBlockingStreamState::Started);
      break;
    case XtBlockingStreamState::Stopped:
      XT_ASSERT(pthread_mutex_lock(&stream->lock.m) == 0);
      XT_ASSERT(pthread_cond_wait(&stream->controlCv.cv, &stream->lock.m) == 0);
      XT_ASSERT(pthread_mutex_unlock(&stream->lock.m) == 0);
      break;
    default:
      XT_FAIL("Unexpected stream state.");
      break;
    }
  }
  XT_FAIL("End of stream callback reached.");
  return nullptr;
}

// ---- linux ----

XtlLinuxBlockingStream::XtlLinuxBlockingStream(bool secondary):
XtBlockingStream(secondary),
lock(),
state(XtBlockingStreamState::Stopped),
respondCv(), 
controlCv() {
  if(!_secondary) {
    pthread_t thread;
    XT_ASSERT(pthread_create(&thread, nullptr, &LinuxOnBlockingBuffer, this) == 0);
  }
}

XtlLinuxBlockingStream::~XtlLinuxBlockingStream() {
  if(!_secondary) 
    SendLinuxBlockingStreamControl(this, XtBlockingStreamState::Closing, XtBlockingStreamState::Closed);
}

XtFault XtlLinuxBlockingStream::Start() {
  if(!_secondary)
    SendLinuxBlockingStreamControl(this, XtBlockingStreamState::Starting, XtBlockingStreamState::Started);
  else {
    ProcessBuffer(true);
    StartStream();
  }
  return 0;
}

XtFault XtlLinuxBlockingStream::Stop() {
  if(_secondary)
    StopStream();
  else
    SendLinuxBlockingStreamControl(this, XtBlockingStreamState::Stopping, XtBlockingStreamState::Stopped);
  return 0;
}

void XtlLinuxBlockingStream::RequestStop() {
  StopStream();
  if(!_secondary) {
    XT_ASSERT(pthread_mutex_lock(&lock.m) == 0);
    state = XtBlockingStreamState::Stopped;
    XT_ASSERT(pthread_cond_signal(&respondCv.cv) == 0);
    XT_ASSERT(pthread_mutex_unlock(&lock.m) == 0);
  }
}

bool XtlLinuxBlockingStream::VerifyOnBuffer(int error, const char* file, int line, const char* func, const char* expr) {
  if(error == 0)
    return true;
  RequestStop();
  XtiTrace(file, line, func, expr);
  XtBuffer buffer = { 0 };
  buffer.error = XtiCreateError(GetSystem(), error);
  OnBuffer(&buffer);
  return false;
}

#endif // __linux__