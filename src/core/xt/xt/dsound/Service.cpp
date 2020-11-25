#if XT_ENABLE_DSOUND
#include <xt/dsound/Fault.hpp>
#include <xt/private/Services.hpp>

XtServiceError
XtiGetDSoundError(XtFault fault)
{
  XtServiceError result;
  result.text = XtiGetDSoundFaultText(fault);
  result.cause = XtiGetDSoundFaultCause(fault);
  return result;
}

#endif // XT_ENABLE_DSOUND