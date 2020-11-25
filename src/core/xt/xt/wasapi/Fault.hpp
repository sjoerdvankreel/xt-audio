#ifndef XT_WASAPI_FAULT_HPP
#define XT_WASAPI_FAULT_HPP

#include <xt/audio/Enums.h>
#include <xt/private/Shared.hpp>

char const* 
XtiGetWasapiFaultText(XtFault fault);
XtCause 
XtiGetWasapiFaultCause(XtFault fault);

#endif // XT_WASAPI_FAULT_HPP