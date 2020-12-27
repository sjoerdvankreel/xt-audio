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
  void** hints;
  auto result = std::make_unique<AlsaDeviceList>();
  XT_VERIFY_ALSA(snd_device_name_hint(-1, "pcm", &hints);
  for(size_t i = 0; i < result->_hints[i] != nullptr; i++)
  {
    XtAlsaDeviceInfo info;
    info.name = XtiGetAlsaHint(_hints[i], "NAME");
    std::string ioid = XtiGetAlsaHint(_hints[i], "IOID");
    if(ioid == "Input" || ioid == "")
    {
      info.type = XtAlsaType::InputRw;
      result->_devices.push_back(info);
      info.type = XtAlsaType::InputMMap;
      result->_devices.push_back(info);
    }    
    if(ioid == "Output" || ioid == "")
    {
      info.type = XtAlsaType::OutputRw;
      result->_devices.push_back(info);
      info.type = XtAlsaType::OutputMMap;
      result->_devices.push_back(info);
    }    
  }
  XT_VERIFY_ALSA(snd_device_name_free_hint(hints));
  *list = result.release();
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