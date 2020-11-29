#ifndef XT_LINUX_HPP
#define XT_LINUX_HPP
#ifdef __linux__

#include <xt/Private.hpp>
#include <pthread.h>

// ---- linux ----

struct XtlLinuxBlockingStream: public XtBlockingStream {
  
  XT_IMPLEMENT_CALLBACK_OVER_BLOCKING_STREAM();

  XtlLinuxBlockingStream(bool secondary);
  ~XtlLinuxBlockingStream();
  bool VerifyOnBuffer(int error, XtLocation const& location, const char* expr);
};

#endif // __linux__
#endif // XT_LINUX_HPP