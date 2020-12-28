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
AlsaDevice::SupportsAccess(XtBool interleaved, XtBool* supports) const
{ 
  XtAlsaPcm pcm;
  snd_pcm_hw_params_t* params;
  snd_pcm_hw_params_alloca(&params);
  XT_VERIFY_ALSA(XtiAlsaOpenPcm(_info, &pcm));
  XT_VERIFY_ALSA(snd_pcm_hw_params_any(pcm.pcm, params));
  auto access = XtiGetAlsaAccess(_info.type, interleaved);
  *supports = snd_pcm_hw_params_test_access(pcm.pcm, params, access) == 0;
  return 0;
}

XtFault
AlsaDevice::GetChannelName(XtBool output, int32_t index, char* buffer, int32_t* size) const
{ 
  return 0;
}

XtFault
AlsaDevice::GetChannelCount(XtBool output, int32_t* count) const
{
  return 0;
}

XtFault
AlsaDevice::SupportsFormat(XtFormat const* format, XtBool* supports) const
{
  return 0;
}

XtFault
AlsaDevice::GetBufferSize(XtFormat const* format, XtBufferSize* size) const
{
  return 0;
}

XtFault
AlsaDevice::OpenBlockingStream(XtBlockingParams const* params, XtBlockingStream** stream)
{
  return 0;
}

#endif // XT_ENABLE_ALSA