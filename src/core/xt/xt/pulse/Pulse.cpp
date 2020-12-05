#if XT_ENABLE_PULSE
#include <xt/pulse/Shared.hpp>
#include <xt/private/Services.hpp>
#include <xt/api/private/Platform.hpp>
#include <pulse/pulseaudio.h>

std::unique_ptr<XtService>
XtiCreatePulseService()
{ return std::make_unique<PulseService>(); }

XtServiceError
XtiGetPulseError(XtFault fault)
{
  XtServiceError result;
  result.text = pa_strerror(fault);
  result.cause = XtiGetPulseFaultCause(fault);
  return result;
}

pa_sample_format
XtiSampleToPulse(XtSample sample)
{
  switch(sample)
  {
  case XtSampleUInt8: return PA_SAMPLE_U8; 
  case XtSampleInt16: return PA_SAMPLE_S16LE; 
  case XtSampleInt24: return PA_SAMPLE_S24LE;
  case XtSampleInt32: return PA_SAMPLE_S32LE; 
  case XtSampleFloat32: return PA_SAMPLE_FLOAT32LE;
  default: return XT_ASSERT(false), PA_SAMPLE_U8;
  }
}

XtCause
XtiGetPulseFaultCause(XtFault fault)
{ 
  switch(fault) 
  {
  case PA_ERR_BUSY:
  case PA_ERR_EXIST:
  case PA_ERR_KILLED:
  case PA_ERR_NOENTITY: return XtCauseEndpoint;
  case PA_ERR_VERSION:
  case PA_ERR_INVALIDSERVER:
  case PA_ERR_MODINITFAILED:
  case PA_ERR_CONNECTIONREFUSED:
  case PA_ERR_CONNECTIONTERMINATED: return XtCauseService;
  default: return XtCauseUnknown;
  }
}

int
XtiCreatePulseDefaultClient(XtBool output, pa_simple** pa)
{
  int error = 0;
  pa_sample_spec spec;
  spec.rate = XtiPaDefaultRate;
  spec.channels = XtiPaDefaultChannels;
  spec.format = XtiSampleToPulse(XtiPaDefaultSample);
  char const* id = XtPlatform::instance->_id.c_str();
  auto dir = output? PA_STREAM_PLAYBACK: PA_STREAM_RECORD;
  *pa = pa_simple_new(nullptr, id, dir, nullptr, id, &spec, nullptr, nullptr, &error);
  if(error == 0) return 0;
  if(*pa != nullptr) pa_simple_free(*pa);
  return error;
}

#endif // XT_ENABLE_PULSE