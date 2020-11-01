#ifndef _XT_LINUX_HPP
#define _XT_LINUX_HPP
#ifdef __linux__

#include "XtPrivate.hpp"
#include <pthread.h>

// ---- linux ----

void XtlInitJack();
void XtlInitAlsa();
void XtlTerminateJack();
void XtlTerminateAlsa();
XtCause XtlPosixErrorToCause(XtFault fault);

struct XtlMutex {
  pthread_mutex_t m;
  XtlMutex(const XtlMutex&) = delete;
  XtlMutex& operator=(const XtlMutex&) = delete;
  ~XtlMutex() { XT_ASSERT(pthread_mutex_destroy(&m) == 0); }
  XtlMutex(): m() { XT_ASSERT(pthread_mutex_init(&m, nullptr) == 0); }
};

struct XtlCondition {
  pthread_cond_t cv;
  XtlCondition(const XtlCondition&) = delete;
  XtlCondition& operator=(const XtlCondition&) = delete;
  ~XtlCondition() { XT_ASSERT(pthread_cond_destroy(&cv) == 0); }
  XtlCondition(): cv() { XT_ASSERT(pthread_cond_init(&cv, nullptr) == 0); }
};

struct XtlLinuxBlockingStream: public XtBlockingStream {
  XtlMutex lock;
  XtlCondition respondCv;
  XtlCondition controlCv;
  XtBlockingStreamState state;
  XT_IMPLEMENT_CALLBACK_OVER_BLOCKING_STREAM();

  XtlLinuxBlockingStream(bool secondary);
  ~XtlLinuxBlockingStream();
  bool VerifyStreamCallback(int error, const char* file, int line, const char* func, const char* expr);
};

#endif // __linux__
#endif // _XT_LINUX_HPP