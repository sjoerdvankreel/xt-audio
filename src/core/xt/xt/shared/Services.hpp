#ifndef XT_SHARED_SERVICES_HPP
#define XT_SHARED_SERVICES_HPP

#include <xt/api/Enums.h>
#include <xt/api/Shared.h>
#include <xt/api/Structs.h>
#include <xt/shared/Shared.hpp>
#include <xt/private/Service.hpp>

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

#endif // XT_SHARED_SERVICES_HPP