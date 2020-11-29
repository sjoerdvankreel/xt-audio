#ifndef XT_PRIVATE_BLOCKING_STREAM_WIN32_HPP
#define XT_PRIVATE_BLOCKING_STREAM_WIN32_HPP
#ifdef _WIN32
#define NOMINMAX 1
#include <xt/private/Win32.hpp>

struct XtBlockingStreamImpl
{
  XtEvent const respond;
  XtEvent const control;
  XtCriticalSection lock;
  XtBlockingStreamImpl(bool secondary);
};

#endif // _WIN32
#endif // XT_PRIVATE_BLOCKING_STREAM_WIN32_HPP