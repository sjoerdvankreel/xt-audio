#if XT_ENABLE_ALSA
#include <xt/backend/alsa/Shared.hpp>
#include <xt/backend/alsa/Private.hpp>

#include <memory>
#include <cstring>

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
  XT_VERIFY_ALSA(snd_device_name_hint(-1, "pcm", &hints));
  for(size_t i = 0; hints[i] != nullptr; i++)
  {
    XtAlsaDeviceInfo info;
    info.name = XtiGetAlsaHint(hints[i], "NAME");
    std::string ioid = XtiGetAlsaHint(hints[i], "IOID");
    if(ioid == "Input" || ioid == "" && ((flags & XtEnumFlagsInput) != 0))
    {
      info.type = XtAlsaType::InputRw;
      result->_devices.push_back(info);
      info.type = XtAlsaType::InputMMap;
      result->_devices.push_back(info);
    }    
    if(ioid == "Output" || ioid == "" && ((flags & XtEnumFlagsOutput) != 0))
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
  XtDeviceList* l;
  int32_t index = -1;
  auto flags = output? XtEnumFlagsOutput: XtEnumFlagsInput;
  XT_VERIFY_ALSA(OpenDeviceList(flags, &l));
  std::unique_ptr<AlsaDeviceList> list(&dynamic_cast<AlsaDeviceList&>(*l));
  l = nullptr;
  for(size_t i = 0; i < list->_devices.size(); i++)
    if(list->_devices[i].name == "default")
    {
      index = i;
      break;
    }
  if(index == -1 && !list->_devices.empty()) index = 0;
  if(index == -1) return 0;
  *valid = XtTrue;
  XtiCopyString(XtiGetAlsaDeviceId(list->_devices[index]).c_str(), buffer, size);
  return 0;
}

XtFault
AlsaService::OpenDevice(char const* id, XtDevice** device) const
{
  return 0;
}

#endif // XT_ENABLE_ALSA