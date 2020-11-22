#ifndef XT_ALSA_FAULT_HPP
#define XT_ALSA_FAULT_HPP

#include <xt/audio/Enums.h>
#include <xt/private/Shared.hpp>

char const* 
XtiGetAlsaFaultText(XtFault fault);
XtCause 
XtiGetAlsaFaultCause(XtFault fault);

#endif // XT_ALSA_FAULT_HPP