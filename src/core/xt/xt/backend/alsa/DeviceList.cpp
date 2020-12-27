#if XT_ENABLE_ALSA
#include <xt/backend/alsa/Shared.hpp>
#include <xt/backend/alsa/Private.hpp>
#include <cstring>

AlsaDeviceList::
~AlsaDeviceList()
{ XT_TRACE_IF(snd_device_name_free_hint(_hints)); }

XtFault
AlsaDeviceList::GetCount(int32_t* count) const
{ *count = _count; return 0; }  
XtFault 
AlsaDeviceList::GetId(int32_t index, char* buffer, int32_t* size) const
{ XtiCopyString(XtiGetAlsaHint(_hints[index], "NAME").c_str(), buffer, size); return 0; }


XtFault
AlsaDeviceList::GetCapabilities(char const* id, XtDeviceCaps* capabilities) const
{
  // TODO
  return 0;
}

XtFault
AlsaDeviceList::GetName(char const* id, char* buffer, int32_t* size) const
{
  for(int32_t i = 0; i < _count; i++)
    if(!strcmp(XtiGetAlsaHint(_hints[i], "NAME").c_str(), id))
      return XtiCopyString(XtiGetAlsaHint(_hints[i], "DESC").c_str(), buffer, size), 0;
  return -EINVAL;
}

#endif // XT_ENABLE_ALSA