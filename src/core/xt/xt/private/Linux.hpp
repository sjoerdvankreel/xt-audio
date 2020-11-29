#ifndef XT_PRIVATE_LINUX_HPP
#define XT_PRIVATE_LINUX_HPP
#ifdef __linux__

#include <xt/api/public/Enums.h>
#include <xt/private/Shared.hpp>
#include <pthread.h>

XtCause
XtiGetPosixFaultCause(XtFault fault);

struct XtMutex
{
  pthread_mutex_t m;
  XtMutex(XtMutex const&) = delete;
  XtMutex& operator=(XtMutex const&) = delete;
  ~XtMutex() { XT_ASSERT(pthread_mutex_destroy(&m) == 0); }
  XtMutex(): m() { XT_ASSERT(pthread_mutex_init(&m, nullptr) == 0); }
};

struct XtCondition
{
  pthread_cond_t cv;
  XtCondition(XtCondition const&) = delete;
  XtCondition& operator=(XtCondition const&) = delete;
  ~XtCondition() { XT_ASSERT(pthread_cond_destroy(&cv) == 0); }
  XtCondition(): cv() { XT_ASSERT(pthread_cond_init(&cv, nullptr) == 0); }
};

#endif // __linux__
#endif // XT_PRIVATE_LINUX_HPP