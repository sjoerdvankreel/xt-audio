#if XT_ENABLE_ALSA
#include <xt/backend/alsa/Shared.hpp>
#include <xt/backend/alsa/Private.hpp>

#include <memory>

XtFault
AlsaService::GetFormatFault() const
{ return -EINVAL; }

XtServiceCaps
AlsaService::GetCapabilities() const
{
  auto result = XtServiceCapsTime
  | XtServiceCapsLatency
  | XtServiceCapsAggregation
  | XtServiceCapsXRunDetection;
  return static_cast<XtServiceCaps>(result);
}

XtFault
AlsaService::OpenDeviceList(XtEnumFlags flags, XtDeviceList** list) const
{  
  auto result = std::make_unique<AlsaDeviceList>();
  result->_count = 0;
  XT_VERIFY_ALSA(snd_device_name_hint(-1, "pcm", &result->_hints));
  while(result->_hints[result->_count] != nullptr) result->_count++;
  return 0;
}

XtFault
AlsaService::GetDefaultDeviceId(XtBool output, XtBool* valid, char* buffer, int32_t* size) const
{
  return 0;
}

XtFault
AlsaService::OpenDevice(char const* id, XtDevice** device) const
{
  return 0;
}

#endif // XT_ENABLE_ALSA