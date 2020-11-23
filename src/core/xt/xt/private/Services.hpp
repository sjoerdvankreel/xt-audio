#ifndef XT_PRIVATE_SERVICES_HPP
#define XT_PRIVATE_SERVICES_HPP

#include <xt/audio/Enums.h>
#include <xt/audio/Shared.h>
#include <xt/private/Shared.hpp>
#include <string>
#include <memory>

#define XT_ASE_Format (static_cast<XtFault>(-993))

std::unique_ptr<XtService>
XtiCreateAlsaService();
char const* 
XtiGetAlsaFaultText(XtFault fault);
XtCause 
XtiGetAlsaFaultCause(XtFault fault);

std::unique_ptr<XtService>
XtiCreateJackService();
char const* 
XtiGetJackFaultText(XtFault fault);
XtCause 
XtiGetJackFaultCause(XtFault fault);

std::unique_ptr<XtService>
XtiCreateAsioService();
char const* 
XtiGetAsioFaultText(XtFault fault);
XtCause 
XtiGetAsioFaultCause(XtFault fault);

std::unique_ptr<XtService>
XtiCreatePulseService();
char const* 
XtiGetPulseFaultText(XtFault fault);
XtCause 
XtiGetPulseFaultCause(XtFault fault);

std::unique_ptr<XtService>
XtiCreateWasapiService();
char const* 
XtiGetWasapiFaultText(XtFault fault);
XtCause 
XtiGetWasapiFaultCause(XtFault fault);

std::unique_ptr<XtService>
XtiCreateDSoundService();
char const* 
XtiGetDSoundFaultText(XtFault fault);
XtCause 
XtiGetDSoundFaultCause(XtFault fault);

#endif // XT_PRIVATE_SERVICES_HPP