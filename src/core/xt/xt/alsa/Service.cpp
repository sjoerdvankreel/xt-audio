#if XT_ENABLE_ALSA
#include <xt/private/Services.hpp>
#include <xt/private/Linux.hpp>
#include <alsa/asoundlib.h>
#include <algorithm>

XtServiceError
XtiGetAlsaError(XtFault fault)
{
  XtServiceError result;
  result.text = snd_strerror(fault);
  result.cause = XtiGetPosixFaultCause(std::abs(static_cast<int>(fault)));
  return result;
}

#endif // XT_ENABLE_ALSA