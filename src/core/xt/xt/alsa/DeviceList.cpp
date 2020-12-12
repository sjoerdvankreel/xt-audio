#if XT_ENABLE_ALSA
#include <xt/alsa/Shared.hpp>
#include <xt/alsa/Private.hpp>
#include <xt/private/Shared.hpp>

AlsaDeviceList::
AlsaDeviceList(void** hints):
_hints(hints) { } 

AlsaDeviceList::
~AlsaDeviceList()
{ XT_ASSERT(snd_device_name_free_hint(_hints) == 0); }

XtFault
AlsaDeviceList::GetCount(int32_t* count) const
{
}

XtFault
AlsaDeviceList::GetDefaultId(XtBool output, XtBool* valid, char* buffer, int32_t* size) const
{
}
  
XtFault 
AlsaDeviceList::GetId(int32_t index, char* buffer, int32_t* size) const
{  
}

XtFault
AsioDeviceList::GetName(char const* id, char* buffer, int32_t* size) const
{
}

#endif // XT_ENABLE_ALSA