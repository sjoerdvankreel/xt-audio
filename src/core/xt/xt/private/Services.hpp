#ifndef XT_PRIVATE_SERVICES_HPP
#define XT_PRIVATE_SERVICES_HPP

#include <xt/audio/Enums.h>
#include <xt/audio/Shared.h>
#include <xt/private/Shared.hpp>
#include <string>
#include <memory>

#define XT_ASE_Format (static_cast<XtFault>(-993))

char const* 
XtiGetAlsaFaultText(XtFault fault);
XtCause 
XtiGetAlsaFaultCause(XtFault fault);
std::unique_ptr<XtService>
XtiCreateAlsaService(std::string const& id, void* window);

char const* 
XtiGetJackFaultText(XtFault fault);
XtCause 
XtiGetJackFaultCause(XtFault fault);
std::unique_ptr<XtService>
XtiCreateJackService(std::string const& id, void* window);

char const* 
XtiGetAsioFaultText(XtFault fault);
XtCause 
XtiGetAsioFaultCause(XtFault fault);
std::unique_ptr<XtService>
XtiCreateAsioService(std::string const& id, void* window);

char const* 
XtiGetPulseFaultText(XtFault fault);
XtCause 
XtiGetPulseFaultCause(XtFault fault);
std::unique_ptr<XtService>
XtiCreatePulseService(std::string const& id, void* window);

char const* 
XtiGetWasapiFaultText(XtFault fault);
XtCause 
XtiGetWasapiFaultCause(XtFault fault);
std::unique_ptr<XtService>
XtiCreateWasapiService(std::string const& id, void* window);

char const* 
XtiGetDSoundFaultText(XtFault fault);
XtCause 
XtiGetDSoundFaultCause(XtFault fault);
std::unique_ptr<XtService>
XtiCreateDSoundService(std::string const& id, void* window);

#endif // XT_PRIVATE_SERVICES_HPP