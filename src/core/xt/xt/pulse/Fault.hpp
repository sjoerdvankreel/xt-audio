#ifndef XT_PULSE_FAULT_HPP
#define XT_PULSE_FAULT_HPP

#include <xt/audio/Enums.h>
#include <xt/private/Shared.hpp>

char const* 
XtiGetPulseFaultText(XtFault fault);
XtCause 
XtiGetPulseFaultCause(XtFault fault);

#endif // XT_PULSE_FAULT_HPP