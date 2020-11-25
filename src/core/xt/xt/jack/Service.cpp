#if XT_ENABLE_JACK
#include <xt/private/Linux.hpp>
#include <xt/private/Services.hpp>
#include <cstring>

XtServiceError
XtiGetJackError(XtFault fault)
{
  XtServiceError result;
  result.text = strerror(fault);
  result.cause = XtiGetPosixFaultCause(fault);
  return result;
}

#endif // XT_ENABLE_JACK