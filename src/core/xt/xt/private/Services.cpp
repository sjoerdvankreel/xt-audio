#include <xt/private/Services.hpp>

#if !XT_ENABLE_ALSA
char const* 
XtiGetAlsaFaultText(XtFault fault) 
{ return nullptr; }
XtCause 
XtiGetAlsaFaultCause(XtFault fault) 
{ return XtCauseUnknown; }
std::unique_ptr<XtService>
XtiCreateAlsaService()
{ return std::unique_ptr<XtService>(); }
#endif // !XT_ENABLE_ALSA

#if !XT_ENABLE_JACK
char const* 
XtiGetJackFaultText(XtFault fault) 
{ return nullptr; }
XtCause 
XtiGetJackFaultCause(XtFault fault) 
{ return XtCauseUnknown; }
std::unique_ptr<XtService>
XtiCreateJackService()
{ return std::unique_ptr<XtService>(); }
#endif // !XT_ENABLE_JACK

#if !XT_ENABLE_ASIO
char const* 
XtiGetAsioFaultText(XtFault fault) 
{ return nullptr; }
XtCause 
XtiGetAsioFaultCause(XtFault fault) 
{ return XtCauseUnknown; }
std::unique_ptr<XtService>
XtiCreateAsioService()
{ return std::unique_ptr<XtService>(); }
#endif // !XT_ENABLE_ASIO

#if !XT_ENABLE_PULSE
char const* 
XtiGetPulseFaultText(XtFault fault) 
{ return nullptr; }
XtCause 
XtiGetPulseFaultCause(XtFault fault) 
{ return XtCauseUnknown; }
std::unique_ptr<XtService>
XtiCreatePulseService()
{ return std::unique_ptr<XtService>(); }
#endif // !XT_ENABLE_PULSE

#if !XT_ENABLE_DSOUND
char const* 
XtiGetDSoundFaultText(XtFault fault) 
{ return nullptr; }
XtCause 
XtiGetDSoundFaultCause(XtFault fault) 
{ return XtCauseUnknown; }
std::unique_ptr<XtService>
XtiCreateDSoundService()
{ return std::unique_ptr<XtService>(); }
#endif // !XT_ENABLE_DSOUND

#if !XT_ENABLE_WASAPI
char const* 
XtiGetWasapiFaultText(XtFault fault) 
{ return nullptr; }
XtCause 
XtiGetWasapiFaultCause(XtFault fault) 
{ return XtCauseUnknown; }
std::unique_ptr<XtService>
XtiCreateWasapiService()
{ return std::unique_ptr<XtService>(); }
#endif // !XT_ENABLE_WASAPI