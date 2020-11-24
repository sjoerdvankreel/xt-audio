#if XT_ENABLE_ALSA

#include <xt/private/Services.hpp>
#include <xt/private/Linux.hpp>

#include <alsa/asoundlib.h>
#include <algorithm>

char const* 
XtiGetAlsaFaultText(XtFault fault)
{ return snd_strerror(fault); }
XtCause 
XtiGetAlsaFaultCause(XtFault fault)
{ return XtiGetPosixFaultCause(std::abs(static_cast<int>(fault))); }

#endif // XT_ENABLE_ALSA