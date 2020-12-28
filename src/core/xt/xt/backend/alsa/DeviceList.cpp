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
  if(!XtiParseAlsaDeviceInfo(id, &info)) return -EINVAL;
  std::ostringstream oss;
  oss << info.name << " (" << XtiGetAlsaNameSuffix(info.type) << ")";
  XtiCopyString(oss.str().c_str(), buffer, size);
  return 0;
}

XtFault
AlsaDeviceList::GetCapabilities(char const* id, XtDeviceCaps* capabilities) const
{
  XtAlsaDeviceInfo info;
  if(!XtiParseAlsaDeviceInfo(id, &info)) return -EINVAL;
  if(info.type == XtAlsaType::InputRw || info.type == XtAlsaType::InputMMap) *capabilities = XtDeviceCapsInput;
  else *capabilities = XtDeviceCapsOutput;
  return 0;
}

#endif // XT_ENABLE_ALSA