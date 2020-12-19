#ifndef XT_SHARED_LINUX_HPP
#define XT_SHARED_LINUX_HPP
#ifdef __linux__
#include <xt/shared/Shared.hpp>

XtCause
XtiGetPosixFaultCause(XtFault fault);

#endif // __linux__
#endif // XT_SHARED_LINUX_HPP