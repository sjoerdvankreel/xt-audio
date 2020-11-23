#ifndef XT_PRIVATE_SHARED_HPP
#define XT_PRIVATE_SHARED_HPP

#include <xt/audio/Enums.h>
#include <cstdint>

typedef uint32_t XtFault;

bool
XtiCalledOnMainThread();
char const* 
XtiGetFaultText(XtSystem system, XtFault fault);
XtCause 
XtiGetFaultCause(XtSystem system, XtFault fault);

#endif // XT_PRIVATE_SHARED_HPP