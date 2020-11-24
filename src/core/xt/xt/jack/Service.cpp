#if XT_ENABLE_JACK

#include <xt/private/Linux.hpp>
#include <xt/private/Services.hpp>

#include <cstring>

char const* 
XtiGetJackFaultText(XtFault fault)
{ return strerror(fault); }
XtCause 
XtiGetJackFaultCause(XtFault fault)
{ return XtiGetPosixFaultCause(fault); }

#endif // XT_ENABLE_JACK