#if XT_ENABLE_JACK
#include <xt/api/private/Platform.hpp>
#include <xt/private/Services.hpp>
#include <cstring>

XtServiceError
XtiGetJackError(XtFault fault)
{
  XtServiceError result;
  result.text = strerror(fault);
  result.cause = XtPlatform::GetPosixFaultCause(fault);
  return result;
}

#endif // XT_ENABLE_JACK