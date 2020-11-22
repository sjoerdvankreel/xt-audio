#ifndef XT_PULSE_FAULT_HPP
#define XT_PULSE_FAULT_HPP

#include <xt/audio/Enums.h>
#include <xt/private/Shared.hpp>

#define XT_PA_ERR_FORMAT (PA_ERR_MAX + 1)

char const* 
XtiGetPulseFaultText(XtFault fault);
XtCause 
XtiGetPulseFaultCause(XtFault fault);

#endif // XT_PULSE_FAULT_HPP