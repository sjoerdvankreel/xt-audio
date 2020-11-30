#ifndef XT_PRIVATE_BLOCKING_STREAM_WIN32_HPP
#define XT_PRIVATE_BLOCKING_STREAM_WIN32_HPP
#ifdef _WIN32
#define NOMINMAX 1
#include <xt/private/BlockingStream.hpp>
#include <xt/private/Win32.hpp>

typedef XtBlockingStreamBase<struct XtWin32BlockingStream>
XtBlockingStream;

struct XtWin32BlockingStream:
public XtBlockingStreamBase<XtWin32BlockingStream>
{
  XtEvent const respond;
  XtEvent const control;
  XtCriticalSection lock;
  XtWin32BlockingStream(bool secondary);
};

#endif // _WIN32
#endif // XT_PRIVATE_BLOCKING_STREAM_WIN32_HPP