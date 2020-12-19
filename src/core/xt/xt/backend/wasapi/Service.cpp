#if XT_ENABLE_WASAPI
#include <xt/wasapi/Fault.hpp>
#include <xt/private/Services.hpp>

XtServiceError
XtiGetWasapiError(XtFault fault)
{
  XtServiceError result;
  result.text = XtiGetWasapiFaultText(fault);
  result.cause = XtiGetWasapiFaultCause(fault);
  return result;
}

#endif // XT_ENABLE_WASAPI