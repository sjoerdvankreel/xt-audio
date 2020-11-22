#ifndef XT_JACK_FAULT_HPP
#define XT_JACK_FAULT_HPP

#include <xt/audio/Enums.h>
#include <xt/private/Shared.hpp>

char const* 
XtiGetJackFaultText(XtFault fault);
XtCause 
XtiGetJackFaultCause(XtFault fault);

#endif // XT_JACK_FAULT_HPP