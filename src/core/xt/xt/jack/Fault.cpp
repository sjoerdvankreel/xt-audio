#include <xt/private/Services.hpp>
#if !XT_ENABLE_JACK

XtCause XtiGetJackFaultCause(XtFault fault) 
{ return XtCauseUnknown; }
char const* XtiGetJackFaultText(XtFault fault) 
{ return nullptr; }

#else // !XT_ENABLE_JACK
#include <xt/private/Linux.hpp>
#include <cstring>

XtCause XtiGetJackFaultCause(XtFault fault)
{ return XtiGetPosixFaultCause(fault); }
char const* XtiGetJackFaultText(XtFault fault)
{ return strerror(fault); }

#endif // !XT_ENABLE_JACK