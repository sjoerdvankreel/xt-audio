#if XT_ENABLE_ASIO
#include <xt/asio/Fault.hpp>
#include <xt/private/Services.hpp>

XtServiceError
XtiGetAsioError(XtFault fault)
{
  XtServiceError result;
  result.text = XtiGetAsioFaultText(fault);
  result.cause = XtiGetAsioFaultCause(fault);
  return result;
}

#endif // XT_ENABLE_ASIO