#if XT_ENABLE_PULSE
#include <xt/pulse/Shared.hpp>
#include <xt/private/Services.hpp>
#include <pulse/pulseaudio.h>

std::unique_ptr<XtService>
XtiCreatePulseService()
{ return std::make_unique<PulseService>(); }

static XtCause
GetPulseFaultCause(XtFault fault)
{ 
  switch(fault) 
  {
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

XtServiceError
XtiGetPulseError(XtFault fault)
{
  XtServiceError result;
  result.text = pa_strerror(fault);
  result.cause = GetPulseFaultCause(fault);
  return result;
}

#endif // XT_ENABLE_PULSE