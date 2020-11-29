#ifndef XT_PRIVATE_SERVICES_HPP
#define XT_PRIVATE_SERVICES_HPP

#include <xt/api/public/Enums.h>
#include <xt/api/public/Shared.h>
#include <xt/api/public/Structs.h>
#include <xt/api/private/Service.hpp>
#include <xt/private/Shared.hpp>
#include <memory>

std::unique_ptr<XtService>
XtiCreateAlsaService();
XtServiceError
XtiGetAlsaError(XtFault fault);

std::unique_ptr<XtService>
XtiCreateJackService();
XtServiceError
XtiGetJackError(XtFault fault);

std::unique_ptr<XtService>
XtiCreateAsioService();
XtServiceError
XtiGetAsioError(XtFault fault);

std::unique_ptr<XtService>
XtiCreatePulseService();
XtServiceError
XtiGetPulseError(XtFault fault);

std::unique_ptr<XtService>
XtiCreateWasapiService();
XtServiceError
XtiGetWasapiError(XtFault fault);

std::unique_ptr<XtService>
XtiCreateDSoundService();
XtServiceError
XtiGetDSoundError(XtFault fault);

#endif // XT_PRIVATE_SERVICES_HPP