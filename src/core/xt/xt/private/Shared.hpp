#ifndef XT_PRIVATE_SHARED_HPP
#define XT_PRIVATE_SHARED_HPP

#include <xt/audio/Enums.h>
#include <cstdint>

typedef uint32_t XtFault;

bool
XtiCalledOnMainThread();
int32_t
XtiGetPopCount64(uint64_t x);
uint32_t
XtiGetErrorFault(XtError error);
XtError
XtiCreateError(XtSystem system, XtFault fault);
char const* 
XtiGetFaultText(XtSystem system, XtFault fault);
XtCause 
XtiGetFaultCause(XtSystem system, XtFault fault);

#endif // XT_PRIVATE_SHARED_HPP