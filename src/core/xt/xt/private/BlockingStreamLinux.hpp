#ifndef XT_PRIVATE_BLOCKING_STREAM_LINUX_HPP
#define XT_PRIVATE_BLOCKING_STREAM_LINUX_HPP
#ifdef __linux__
#include <xt/private/BlockingStream.hpp>
#include <xt/private/Linux.hpp>

typedef XtBlockingStreamBase<struct XtLinuxBlockingStream>
XtBlockingStream;

struct XtLinuxBlockingStream:
public XtBlockingStreamBase<XtLinuxBlockingStream>
{
  XtMutex lock;
  XtCondition respond;
  XtCondition control;
  XtLinuxBlockingStream(bool secondary);
  static void* OnBlockingBuffer(void* user);
};

template struct XtBlockingStreamBase<XtLinuxBlockingStream>;

#endif // __linux__
#endif // XT_PRIVATE_BLOCKING_STREAM_LINUX_HPP