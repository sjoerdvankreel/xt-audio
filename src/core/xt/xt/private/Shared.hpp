#ifndef XT_PRIVATE_SHARED_HPP
#define XT_PRIVATE_SHARED_HPP

#include <xt/audio/Enums.h>
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
XtiGetPulseService();
XtService const*
XtiGetWasapiService();
XtService const*
XtiGetDSoundService();

char const* 
XtiGetFaultText(XtSystem system, XtFault fault);
XtCause 
XtiGetFaultCause(XtSystem system, XtFault fault);

#endif // XT_PRIVATE_SHARED_HPP