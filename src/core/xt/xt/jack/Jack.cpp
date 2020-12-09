#if XT_ENABLE_JACK
#include <xt/jack/Shared.hpp>
#include <xt/jack/Private.hpp>
#include <xt/private/Linux.hpp>

XtServiceError
XtiGetJackError(XtFault fault)
{
  XtServiceError result;
  result.text = strerror(fault);
  result.cause = XtiGetPosixFaultCause(fault);
  return result;
}

#endif // XT_ENABLE_JACK