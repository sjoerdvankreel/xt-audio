#ifndef XT_PRIVATE_BLOCKING_STREAM_LINUX_HPP
#define XT_PRIVATE_BLOCKING_STREAM_LINUX_HPP
#ifdef __linux__
#include <xt/private/Linux.hpp>

struct XtBlockingStreamImpl
{
  XtMutex lock;
  XtCondition respond;
  XtCondition control;
  XtBlockingStreamImpl(bool secondary);
};

#endif // __linux__
#endif // XT_PRIVATE_BLOCKING_STREAM_LINUX_HPP