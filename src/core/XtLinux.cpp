#ifdef __linux__
#include "XtLinux.hpp"
#include <pthread.h>

// ---- local ----

static pthread_t XtlMainThread;
static bool XtlInitialized = false;

extern const XtService* XtiServiceAlsa;
extern const XtService* XtiServiceJack;
extern const XtService* XtiServicePulse;

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

static void* LinuxBlockingStreamCallback(void* user) {

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

// ---- api ----

void XT_CALL XtAudioGetSystems(XtSystem* buffer, int32_t* size) {
  std::vector<XtSystem> systems;
  if(XtiServiceAlsa != nullptr) systems.push_back(XtSystemAlsa);
  if(XtiServiceJack != nullptr) systems.push_back(XtSystemJack);
  if(XtiServicePulse != nullptr) systems.push_back(XtSystemPulse);
  auto count = static_cast<int32_t>(systems.size());
  if(buffer == nullptr) 
    *size = count;
  else
    memcpy(buffer, systems.data(), std::min(*size, count)*sizeof(XtSystem));
}

const XtService* XT_CALL XtAudioGetService(XtSystem system) {
  XT_ASSERT(XtSystemAlsa <= system && system <= XtSystemWasapi);
  switch(system) {
  case XtSystemAlsa: return XtiServiceAlsa;
  case XtSystemJack: return XtiServiceJack;
  case XtSystemPulse: return XtiServicePulse;
  case XtSystemAsio:
  case XtSystemWasapi:
  case XtSystemDirectSound return nullptr;
  default: return XT_FAIL("Unknown system."), nullptr;
  }
}

XtSystem XT_CALL XtAudioSetupToSystem(XtSetup setup) {
  switch(setup) {
  case XtSetupProAudio: return XtSystemJack;
  case XtSetupSystemAudio: return XtSystemAlsa;
  case XtSetupConsumerAudio: return XtSystemPulse;
  default: return XT_FAIL("Unknown setup."), XtSystemPulse;
  }
}

// ---- internal ----

void XtiTerminatePlatform() { 
  XtlTerminateJack();
  XtlTerminateAlsa();
  XtlInitialized = false;
}

void XtiInitPlatform(void* wnd) {
  XT_ASSERT(!XtlInitialized);
  XtlMainThread = pthread_self();
  XtlInitAlsa();
  XtlInitJack();
  XtlInitialized = true;
}

int32_t XtiLockIncr(volatile int32_t* dest) {
  return __sync_add_and_fetch(dest, 1);
}

int32_t XtiLockDecr(volatile int32_t* dest) {
  return __sync_sub_and_fetch(dest, 1);
}

int32_t XtiCas(volatile int32_t* dest, int32_t exch, int32_t comp) {
  return __sync_val_compare_and_swap(dest, comp, exch);
}

bool XtiCalledOnMainThread() {
  return XtlInitialized && pthread_equal(pthread_self(), XtlMainThread);
}

// ---- linux ----

XtCause XtlPosixErrorToCause(XtFault fault) {
  switch(fault) {
  case ESRCH: return XtCauseService;
  case EINVAL: return XtCauseFormat;
  case EBUSY:
  case ENXIO:
  case EPIPE:
  case ENODEV:
  case ENOENT:
  case ESTRPIPE: return XtCauseEndpoint;
  default: return XtCauseUnknown;
  }
}

XtlLinuxBlockingStream::XtlLinuxBlockingStream(bool secondary):
XtBlockingStream(secondary),
lock(),
state(XtBlockingStreamState::Stopped),
respondCv(), 
controlCv() {
  if(!secondary) {
    pthread_t thread;
    XT_ASSERT(pthread_create(&thread, nullptr, &LinuxBlockingStreamCallback, this) == 0);
  }
}

XtlLinuxBlockingStream::~XtlLinuxBlockingStream() {
  if(!secondary) 
    SendLinuxBlockingStreamControl(this, XtBlockingStreamState::Closing, XtBlockingStreamState::Closed);
}

XtFault XtlLinuxBlockingStream::Start() {
  if(!secondary)
    SendLinuxBlockingStreamControl(this, XtBlockingStreamState::Starting, XtBlockingStreamState::Started);
  else {
    ProcessBuffer(true);
    StartStream();
  }
  return 0;
}

XtFault XtlLinuxBlockingStream::Stop() {
  if(secondary)
    StopStream();
  else
    SendLinuxBlockingStreamControl(this, XtBlockingStreamState::Stopping, XtBlockingStreamState::Stopped);
  return 0;
}

void XtlLinuxBlockingStream::RequestStop() {
  StopStream();
  if(!secondary) {
    XT_ASSERT(pthread_mutex_lock(&lock.m) == 0);
    state = XtBlockingStreamState::Stopped;
    XT_ASSERT(pthread_cond_signal(&respondCv.cv) == 0);
    XT_ASSERT(pthread_mutex_unlock(&lock.m) == 0);
  }
}

bool XtlLinuxBlockingStream::VerifyStreamCallback(int error, const char* file, int line, const char* func, const char* expr) {
  if(error == 0)
    return true;
  RequestStop();
  XtiTrace(file, line, func, expr);
  ProcessCallback(nullptr, nullptr, 0, 0.0, 0, XtFalse, XtiCreateError(GetSystem(), error));
  return false;
}

#endif // __linux__