#if XT_ENABLE_ALSA
#include <xt/backend/alsa/Shared.hpp>
#include <xt/backend/alsa/Private.hpp>

void*
AlsaDevice::GetHandle() const
{ return nullptr; }
XtFault
AlsaDevice::ShowControlPanel()
{ return 0; }
XtFault
AlsaDevice::GetMix(XtBool* valid, XtMix* mix) const
{ return 0; }

XtFault
AlsaDevice::GetChannelCount(XtBool output, int32_t* count) const
{
  int err;
  unsigned val;
  XtAlsaPcm pcm = { 0 };
  bool isOutput = XtiAlsaTypeIsOutput(_info.type);
  if(isOutput != (output != XtFalse)) return 0;
  if((err = XtiAlsaOpenPcm(_info, &pcm)) < 0) return err;  
  XT_VERIFY_ALSA(snd_pcm_hw_params_get_channels_max(pcm.params, &val));
  *count = static_cast<int32_t>(val);
  return 0;
}

XtFault
AlsaDevice::SupportsFormat(XtFormat const* format, XtBool* supports) const
{
  int err;
  XtAlsaPcm pcm = { 0 };
  if((err = XtiAlsaOpenPcm(_info, format, &pcm)) < 0) return -EINVAL;
  *supports = XtTrue;
  return 0;
}

XtFault
AlsaDevice::GetBufferSize(XtFormat const* format, XtBufferSize* size) const
{
  int err;
  XtAlsaPcm pcm = { 0 };
  snd_pcm_uframes_t min, max;
  auto rate = format->mix.rate;
  if((err = XtiAlsaOpenPcm(_info, format, &pcm)) < 0) return -EINVAL;
  XT_VERIFY_ALSA(snd_pcm_hw_params_get_buffer_size_min(pcm.params, &min));
  XT_VERIFY_ALSA(snd_pcm_hw_params_get_buffer_size_max(pcm.params, &max));
  size->min = min * 1000.0 / rate;
  size->max = max * 1000.0 / rate;
  return 0;
}

XtFault
AlsaDevice::SupportsAccess(XtBool interleaved, XtBool* supports) const
{ 
  int err;
  XtAlsaPcm pcm = { 0 };
  if((err = XtiAlsaOpenPcm(_info, &pcm)) < 0) return err;
  auto access = XtiGetAlsaAccess(_info.type, interleaved);
  *supports = snd_pcm_hw_params_test_access(pcm.pcm, pcm.params, access) == 0;
  return 0;
}

XtFault
AlsaDevice::GetChannelName(XtBool output, int32_t index, char* buffer, int32_t* size) const
{ 
  return 0;
}

XtFault
AlsaDevice::OpenBlockingStream(XtBlockingParams const* params, XtBlockingStream** stream)
{
  return 0;
}

#endif // XT_ENABLE_ALSA