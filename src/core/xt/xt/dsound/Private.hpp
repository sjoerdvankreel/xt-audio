#ifndef XT_DSOUND_PRIVATE_HPP
#define XT_DSOUND_PRIVATE_HPP
#if XT_ENABLE_DSOUND

#include <xt/api/public/Enums.h>
#include <xt/private/Shared.hpp>

char const* 
XtiGetDSoundFaultText(XtFault fault);
XtCause 
XtiGetDSoundFaultCause(XtFault fault);

#endif // XT_ENABLE_DSOUND
#endif // XT_DSOUND_PRIVATE_HPP