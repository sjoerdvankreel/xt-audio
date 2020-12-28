#if XT_ENABLE_ALSA
#include <xt/backend/alsa/Shared.hpp>
#include <xt/backend/alsa/Private.hpp>

#include <memory>
#include <cstring>

XtFault
AlsaService::GetFormatFault() const
{ return -EINVAL; }
AlsaService::
AlsaService()
{ XT_ASSERT(snd_lib_error_set_handler(&XtiLogAlsaError) == 0); }

AlsaService::
~AlsaService()
{
  XT_TRACE_IF(snd_lib_error_set_handler(nullptr) != 0);
  XT_TRACE_IF(snd_config_update_free_global() != 0);
}

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
AlsaService::OpenDevice(char const* id, XtDevice** device) const
{
  XtAlsaDeviceInfo info;
  if(!XtiParseAlsaDeviceInfo(id, &info)) return -EINVAL;
  auto result = std::make_unique<AlsaDevice>();
  result->_info = info;
  *device = result.release();
  return 0;
}

XtFault
AlsaService::OpenDeviceList(XtEnumFlags flags, XtDeviceList** list) const
{  
  void** hints;
  std::vector<XtAlsaDeviceInfo> devices;
  XT_VERIFY_ALSA(snd_device_name_hint(-1, "pcm", &hints));
  for(size_t i = 0; hints[i] != nullptr; i++)
  {
    XtAlsaDeviceInfo info;
    info.name = XtiGetAlsaHint(hints[i], "NAME");
    std::string ioid = XtiGetAlsaHint(hints[i], "IOID");
    if((ioid == "Input" || ioid == "") && ((flags & XtEnumFlagsInput) != 0))
    {
      info.type = XtAlsaType::InputRw;
      devices.push_back(info);
      info.type = XtAlsaType::InputMMap;
      devices.push_back(info);
    }    
    if((ioid == "Output" || ioid == "") && ((flags & XtEnumFlagsOutput) != 0))
    {
      info.type = XtAlsaType::OutputRw;
      devices.push_back(info);
      info.type = XtAlsaType::OutputMMap;
      devices.push_back(info);
    }    
  }
  XT_VERIFY_ALSA(snd_device_name_free_hint(hints));

  int err;
  auto result = std::make_unique<AlsaDeviceList>();
  for(size_t i = 0; i < devices.size(); i++) 
  {
    XtAlsaPcm pcm = { 0 };
    if((err = XtiAlsaOpenPcm(devices[i], &pcm)) < 0) continue;
    result->_devices.push_back(devices[i]);
  }
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

#endif // XT_ENABLE_ALSA