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
AlsaDeviceList::GetCapabilities(char const* id, XtDeviceCaps* capabilities) const
{
  // TODO
  return 0;
}

XtFault
AlsaDeviceList::GetName(char const* id, char* buffer, int32_t* size) const
{
  int32_t index = -1;
  for(int32_t i = 0; i < _count; i++)
    if(!strcmp(XtiGetAlsaHint(_hints[i], "NAME").c_str(), id))
    {
      index = i;
      break;
    }
  
  if(index == -1) return -EINVAL;
  std::ostringstream oss;
  oss << id << " (" << XtiGetAlsaHint(_hints[index], "IOID") << ")";
  XtiCopyString(oss.str().c_str(), buffer, size);
  return 0;
}

#endif // XT_ENABLE_ALSA