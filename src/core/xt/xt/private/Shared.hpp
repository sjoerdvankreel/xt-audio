#ifndef XT_PRIVATE_SHARED_HPP
#define XT_PRIVATE_SHARED_HPP

#include <xt/audio/Enums.h>
#include <xt/audio/Shared.h>
#include <string>
#include <memory>
#include <cstdint>

typedef uint32_t XtFault;

bool
XtiCalledOnMainThread();
char const* 
XtiGetFaultText(XtSystem system, XtFault fault);
XtCause 
XtiGetFaultCause(XtSystem system, XtFault fault);

std::unique_ptr<XtService>
XtiCreateAlsaService(std::string const& id, void* window);
std::unique_ptr<XtService>
XtiCreateJackService(std::string const& id, void* window);
std::unique_ptr<XtService>
XtiCreateAsioService(std::string const& id, void* window);
std::unique_ptr<XtService>
XtiCreatePulseService(std::string const& id, void* window);
std::unique_ptr<XtService>
XtiCreateWasapiService(std::string const& id, void* window);
std::unique_ptr<XtService>
XtiCreateDSoundService(std::string const& id, void* window);

#endif // XT_PRIVATE_SHARED_HPP