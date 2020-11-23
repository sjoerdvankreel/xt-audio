#include <xt/private/Services.hpp>
#if !XT_ENABLE_PULSE

XtCause XtiGetPulseFaultCause(XtFault fault) 
{ return XtCauseUnknown; }
char const* XtiGetPulseFaultText(XtFault fault) 
{ return nullptr; }

#else // !XT_ENABLE_PULSE
#include <pulse/pulseaudio.h>

char const*
XtiGetPulseFaultText(XtFault fault)
{ return pa_strerror(fault); }

XtCause
XtiGetPulseFaultCause(XtFault fault)
{ 
  switch(fault) {
  case PA_ERR_BUSY:
  case PA_ERR_EXIST:
  case PA_ERR_KILLED:
  case PA_ERR_NOENTITY: return XtCauseEndpoint;
  case PA_ERR_VERSION:
  case PA_ERR_INVALIDSERVER:
  case PA_ERR_MODINITFAILED:
  case PA_ERR_CONNECTIONREFUSED:
  case PA_ERR_CONNECTIONTERMINATED: return XtCauseService;
  default: return XtCauseUnknown;
  }
}

#endif // !XT_ENABLE_PULSE