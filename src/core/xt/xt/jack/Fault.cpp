#include <xt/private/Services.hpp>
#if !XT_ENABLE_JACK

char const* 
XtiGetJackFaultText(XtFault fault) 
{ return nullptr; }
XtCause 
XtiGetJackFaultCause(XtFault fault) 
{ return XtCauseUnknown; }

#else // !XT_ENABLE_JACK
#include <xt/private/Linux.hpp>
#include <cstring>

char const* 
XtiGetJackFaultText(XtFault fault)
{ return strerror(fault); }
XtCause 
XtiGetJackFaultCause(XtFault fault)
{ return XtiGetPosixFaultCause(fault); }

#endif // !XT_ENABLE_JACK