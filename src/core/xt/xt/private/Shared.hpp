#ifndef XT_PRIVATE_SHARED_HPP
#define XT_PRIVATE_SHARED_HPP

#include <xt/audio/Shared.h>
#include <cstdint>

typedef uint32_t XtFault;

XtService const*
XtiGetAlsaService();
XtService const*
XtiGetJackService();
XtService const*
XtiGetAsioService();
XtService const*
XtiGetWasapiService();
XtService const*
XtiGetPulseAudioService();
XtService const*
XtiGetDirectSoundService();

#endif // XT_PRIVATE_SHARED_HPP