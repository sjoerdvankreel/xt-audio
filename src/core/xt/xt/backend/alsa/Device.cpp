#if XT_ENABLE_ALSA
#include <xt/backend/alsa/Shared.hpp>
#include <xt/backend/alsa/Private.hpp>

#include <memory>
#include <algorithm>

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
  *count = std::min(64, static_cast<int32_t>(val));
  return 0;
}

XtFault
AlsaDevice::SupportsFormat(XtFormat const* format, XtBool* supports) const
{
  int err;
  XtAlsaPcm pcm = { 0 };
  if((err = XtiAlsaOpenPcm(_info, format, &pcm)) < 0) return 0;
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
  size->current = size->min;
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
  auto pos = static_cast<snd_pcm_chmap_position>(index + SND_CHMAP_FL);
  if(pos > SND_CHMAP_LAST) pos = SND_CHMAP_UNKNOWN;
  XtiCopyString(snd_pcm_chmap_long_name(pos), buffer, size);
  return 0;
}

XtFault
AlsaDevice::OpenBlockingStream(XtBlockingParams const* params, XtBlockingStream** stream)
{
  int err;
  snd_pcm_uframes_t min;
  snd_pcm_uframes_t max;
  snd_pcm_uframes_t buffer;
  snd_pcm_sw_params_t* swParams;

  snd_pcm_sw_params_alloca(&swParams);
  auto result = std::make_unique<AlsaStream>();
  if((err = XtiAlsaOpenPcm(_info, &params->format, &result->_pcm)) < 0) return err;
  auto access = XtiGetAlsaAccess(_info.type, params->interleaved);
  XT_VERIFY_ALSA(snd_pcm_hw_params_set_access(result->_pcm.pcm, result->_pcm.params, access));
  XT_VERIFY_ALSA(snd_pcm_hw_params_get_buffer_size_min(result->_pcm.params, &min));
  XT_VERIFY_ALSA(snd_pcm_hw_params_get_buffer_size_max(result->_pcm.params, &max));
  buffer = params->bufferSize / 1000.0 * params->format.mix.rate;
  buffer = std::clamp(buffer, min, max);
  XT_VERIFY_ALSA(snd_pcm_hw_params_set_buffer_size_near(result->_pcm.pcm, result->_pcm.params, &buffer));  
  XT_VERIFY_ALSA(snd_pcm_hw_params(result->_pcm.pcm, result->_pcm.params));
  XT_VERIFY_ALSA(snd_pcm_sw_params_current(result->_pcm.pcm, swParams));
  XT_VERIFY_ALSA(snd_pcm_sw_params_set_tstamp_mode(result->_pcm.pcm, swParams, SND_PCM_TSTAMP_ENABLE));
  XT_VERIFY_ALSA(snd_pcm_sw_params(result->_pcm.pcm, swParams));
  result->_processed = 0;
  result->_frames = buffer;
  result->_type = _info.type;
  *stream = result.release();
  return 0;
}

#endif // XT_ENABLE_ALSA