#include <xt/private/Services.hpp>
#if !XT_ENABLE_ALSA

char const* 
XtiGetAlsaFaultText(XtFault fault) 
{ return nullptr; }
XtCause 
XtiGetAlsaFaultCause(XtFault fault) 
{ return XtCauseUnknown; }

#else // !XT_ENABLE_ALSA
#include <xt/private/Linux.hpp>
#include <alsa/asoundlib.h>
#include <algorithm>

char const* 
XtiGetAlsaFaultText(XtFault fault)
{ return snd_strerror(fault); }
XtCause 
XtiGetAlsaFaultCause(XtFault fault)
{ return XtiGetPosixFaultCause(std::abs(static_cast<int>(fault))); }

#endif // !XT_ENABLE_ALSA