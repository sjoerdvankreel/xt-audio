#if XT_ENABLE_ALSA
#include <xt/backend/alsa/Shared.hpp>
#include <xt/backend/alsa/Private.hpp>

#include <cstring>
#include <sstream>

XtFault
AlsaDeviceList::GetCount(int32_t* count) const
{ *count = static_cast<int32_t>(_devices.size()); return 0; }  
XtFault 
AlsaDeviceList::GetId(int32_t index, char* buffer, int32_t* size) const
{ XtiCopyString(XtiGetAlsaDeviceId(_devices[index]).c_str(), buffer, size); return 0; }

XtFault
AlsaDeviceList::GetName(char const* id, char* buffer, int32_t* size) const
{
  XtAlsaDeviceInfo info;
  if(!XtiParseAlsaDeviceInfo(id, &info)) return -ENODEV;
  std::ostringstream oss;
  oss << info.name << " (" << XtiGetAlsaNameSuffix(info.type) << ")";
  XtiCopyString(oss.str().c_str(), buffer, size);
  return 0;
}

XtFault
AlsaDeviceList::GetCapabilities(char const* id, XtDeviceCaps* capabilities) const
{
  int flags = 0;
  XtAlsaPcm pcm = { 0 };
  XtAlsaDeviceInfo info;
  if(!XtiParseAlsaDeviceInfo(id, &info)) return -ENODEV;
  XT_VERIFY_ALSA(XtiAlsaOpenPcm(info, &pcm));
  auto pcmType = snd_pcm_type(pcm.pcm);
  if(info.type == XtAlsaType::InputRw || info.type == XtAlsaType::InputMMap) flags |= XtDeviceCapsInput;
  else flags |= XtDeviceCapsOutput;
  if(pcmType == SND_PCM_TYPE_HW) flags |= XtDeviceCapsHwDirect;
  *capabilities = static_cast<XtDeviceCaps>(flags);
  return 0;
}

#endif // XT_ENABLE_ALSA