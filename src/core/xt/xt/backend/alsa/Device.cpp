#if XT_ENABLE_ALSA
#include <xt/backend/alsa/Shared.hpp>
#include <xt/backend/alsa/Private.hpp>

XtFault
AlsaDevice::ShowControlPanel()
{ 
  return 0;
}

XtFault
AlsaDevice::GetMix(XtBool* valid, XtMix* mix) const
{ 
  return 0;
}

void*
AlsaDevice::GetHandle() const
{ 
  return nullptr;
}

XtFault
AlsaDevice::SupportsAccess(XtBool interleaved, XtBool* supports) const
{ 
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