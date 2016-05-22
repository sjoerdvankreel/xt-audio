#ifdef __linux__
#include "xt-linux.hpp"
#include <pthread.h>

/* Copyright (C) 2015-2016 Sjoerd van Kreel.
 *
 * This file is part of XT-Audio.
 *
 * XT-Audio is free software: you can redistribute it and/or modify it under the 
 * terms of the GNU Lesser General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later version.
 *
 * XT-Audio is distributed in the hope that it will be useful, but WITHOUT ANY 
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with XT-Audio. If not, see<http://www.gnu.org/licenses/>.
 */

// ---- local ----

static pthread_t XtlMainThread;
static bool XtlInitialized = false;

static XtStreamState ReadLinuxStreamState(
  XtlLinuxStream* stream) {

  XtStreamState result;
  XT_ASSERT(pthread_mutex_lock(&stream->lock.m) == 0);
  result = stream->state;
  XT_ASSERT(pthread_mutex_unlock(&stream->lock.m) == 0);
  return result;
}

static void ReceiveLinuxStreamControl(
  XtlLinuxStream* stream, XtStreamState state) {

  XT_ASSERT(pthread_mutex_lock(&stream->lock.m) == 0);
  stream->state = state;
  XT_ASSERT(pthread_cond_signal(&stream->respondCv.cv) == 0);
  XT_ASSERT(pthread_mutex_unlock(&stream->lock.m) == 0);
}

static void SendLinuxStreamControl(
  XtlLinuxStream* stream, XtStreamState from, XtStreamState to) {

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

static void* LinuxStreamCallback(void* user) {

  int policy;
  int maxPriority;
  int startPriority;
  XtStreamState state;
  struct sched_param param;
  auto stream = static_cast<XtlLinuxStream*>(user);

  XT_ASSERT(pthread_getschedparam(pthread_self(), &policy, &param) == 0);
  startPriority = param.sched_priority;
  maxPriority = sched_get_priority_max(policy);
  while((state = ReadLinuxStreamState(stream)) != XtStreamStateClosed) {
    switch(state) {
    case XtStreamStateStarted:
      stream->ProcessBuffer(false);
      break;
    case XtStreamStateClosing:
      ReceiveLinuxStreamControl(stream, XtStreamStateClosed);
      return nullptr;
    case XtStreamStateStopping:
      stream->StopStream();
      param.sched_priority = startPriority;
      XT_ASSERT(pthread_setschedparam(pthread_self(), policy, &param) == 0);
      ReceiveLinuxStreamControl(stream, XtStreamStateStopped);
      break;
    case XtStreamStateStarting:
      stream->ProcessBuffer(true);
      param.sched_priority = maxPriority;
      XT_ASSERT(pthread_setschedparam(pthread_self(), policy, &param) == 0);
      stream->StartStream();
      ReceiveLinuxStreamControl(stream, XtStreamStateStarted);
      break;
    case XtStreamStateStopped:
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

XtBool XT_CALL XtAudioIsWin32(void) {
  return XtFalse;
}

int32_t XT_CALL XtAudioGetServiceCount(void) {
  return 3; 
}

const XtService* XT_CALL XtAudioGetServiceBySystem(XtSystem system) {
  switch(system) {
  case XtSystemAlsa: return XtiServiceAlsa;
  case XtSystemJack: return XtiServiceJack;
  case XtSystemPulse: return XtiServicePulse;
  default: return XT_FAIL("Unknown system."), nullptr;
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

int32_t XtiCas(volatile int32_t* dest, int32_t exch, int32_t comp) {
  return __sync_val_compare_and_swap (dest, comp, exch);
}

bool XtiCalledOnMainThread() {
  return XtlInitialized && pthread_equal(pthread_self(), XtlMainThread);
}

XtSystem XtiSetupToSystem(XtSetup setup) {
  switch(setup) {
  case XtSetupProAudio: return XtSystemJack;
  case XtSetupSystemAudio: return XtSystemAlsa;
  case XtSetupConsumerAudio: return XtSystemPulse;
  default: return XT_FAIL("Unknown setup."), XtSystemPulse;
  }
}

XtSystem XtiIndexToSystem(int32_t index) {
  switch(index) {
  case 0: return XtSystemPulse;
  case 1: return XtSystemAlsa;
  case 2: return XtSystemJack;
  default: return XT_FAIL("Unknown index."), XtSystemPulse;
  }
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

XtlLinuxStream::XtlLinuxStream():
lock(), state(XtStreamStateStopped), respondCv(), controlCv() {
  pthread_t thread;
  XT_ASSERT(pthread_create(&thread, nullptr, &LinuxStreamCallback, this) == 0);
}

XtlLinuxStream::~XtlLinuxStream() {
  SendLinuxStreamControl(this, XtStreamStateClosing, XtStreamStateClosed);
}

XtFault XtlLinuxStream::Start() {
  SendLinuxStreamControl(this, XtStreamStateStarting, XtStreamStateStarted);
  return 0;
}

XtFault XtlLinuxStream::Stop() {
  SendLinuxStreamControl(this, XtStreamStateStopping, XtStreamStateStopped);
  return 0;
}

bool XtlLinuxStream::VerifyStreamCallback(int error, const char* file, int line, const char* func, const char* expr) {
  if(error == 0)
    return true;
  StopStream();
  XtiTrace(XtLevelError, file, line, func, expr);
  ProcessCallback(nullptr, nullptr, 0, 0.0, 0, XtFalse, XtiCreateError(XtStreamGetSystem(this), error));
  XT_ASSERT(pthread_mutex_lock(&lock.m) == 0);
  state = XtStreamStateStopped;
  XT_ASSERT(pthread_cond_signal(&respondCv.cv) == 0);
  XT_ASSERT(pthread_mutex_unlock(&lock.m) == 0);
  return false;
}

#endif // __linux__