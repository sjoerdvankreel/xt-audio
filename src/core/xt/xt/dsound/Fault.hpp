#ifndef XT_DSOUND_FAULT_HPP
#define XT_DSOUND_FAULT_HPP

#include <xt/audio/Enums.h>
#include <xt/private/Shared.hpp>

char const* 
XtiGetDirectSoundFaultText(XtFault fault);
XtCause 
XtiGetDirectSoundFaultCause(XtFault fault);

#endif // XT_DSOUND_FAULT_HPP