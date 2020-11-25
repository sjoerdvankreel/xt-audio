#include <xt/private/Services.hpp>
#include <cassert>

#if !XT_ENABLE_ALSA
std::unique_ptr<XtService>
XtiCreateAlsaService()
{ return std::unique_ptr<XtService>(); }
XtServiceError
XtiGetAlsaError(XtFault fault) 
{ return assert(false), XtServiceError(); }
#endif // !XT_ENABLE_ALSA

#if !XT_ENABLE_JACK
std::unique_ptr<XtService>
XtiCreateJackService()
{ return std::unique_ptr<XtService>(); }
XtServiceError
XtiGetJackError(XtFault fault) 
{ return assert(false), XtServiceError(); }
#endif // !XT_ENABLE_JACK

#if !XT_ENABLE_ASIO
std::unique_ptr<XtService>
XtiCreateAsioService()
{ return std::unique_ptr<XtService>(); }
XtServiceError
XtiGetAsioError(XtFault fault) 
{ return assert(false), XtServiceError(); }
#endif // !XT_ENABLE_ASIO

#if !XT_ENABLE_PULSE
std::unique_ptr<XtService>
XtiCreatePulseService()
{ return std::unique_ptr<XtService>(); }
XtServiceError
XtiGetPulseError(XtFault fault) 
{ return assert(false), XtServiceError(); }
#endif // !XT_ENABLE_PULSE

#if !XT_ENABLE_DSOUND
std::unique_ptr<XtService>
XtiCreateDSoundService()
{ return std::unique_ptr<XtService>(); }
XtServiceError
XtiGetDSoundError(XtFault fault) 
{ return assert(false), XtServiceError(); }
#endif // !XT_ENABLE_DSOUND

#if !XT_ENABLE_WASAPI
std::unique_ptr<XtService>
XtiCreateWasapiService()
{ return std::unique_ptr<XtService>(); }
XtServiceError
XtiGetWasapiError(XtFault fault) 
{ return assert(false), XtServiceError(); }
#endif // !XT_ENABLE_WASAPI