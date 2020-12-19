#if XT_ENABLE_ALSA
#include <xt/alsa/Shared.hpp>
#include <xt/alsa/Private.hpp>
#include <xt/private/Shared.hpp>

XtFault
AlsaDeviceList::GetCount(int32_t* count) const
{ *count = _count; return 0; }

AlsaDeviceList::
AlsaDeviceList(void** hints, size_t count):
_hints(hints), _count(count) { } 
AlsaDeviceList::
~AlsaDeviceList()
{ XT_ASSERT(snd_device_name_free_hint(_hints) == 0); }


XtFault
AlsaDeviceList::GetDefaultId(XtBool output, XtBool* valid, char* buffer, int32_t* size) const
{
}
  
XtFault 
AlsaDeviceList::GetId(int32_t index, char* buffer, int32_t* size) const
{  
}

XtFault
AlsaDeviceList::GetName(char const* id, char* buffer, int32_t* size) const
{
}

#endif // XT_ENABLE_ALSA