#ifndef XT_PRIVATE_LINUX_HPP
#define XT_PRIVATE_LINUX_HPP
#ifdef __linux__

#include <xt/audio/Enums.h>
#include <xt/private/Shared.hpp>

XtCause
XtiGetPosixFaultCause(XtFault fault);

#endif // __linux__
#endif // XT_PRIVATE_LINUX_HPP