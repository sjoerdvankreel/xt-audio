#if XT_ENABLE_PULSE
#include <xt/pulse/Fault.hpp>
#include <pulse/pulseaudio.h>

XtServiceError
XtiGetPulseServiceError(XtFault fault)
{
  XtServiceError result;
  result.text = pa_strerror(fault);
  result.cause = XtiGetPulseFaultCause(fault);
  return result;
}

#endif // XT_ENABLE_PULSE