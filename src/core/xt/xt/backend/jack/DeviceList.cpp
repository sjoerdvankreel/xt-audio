#if XT_ENABLE_JACK
#include <xt/backend/jack/Shared.hpp>
#include <xt/backend/jack/Private.hpp>

XtFault
JackDeviceList::GetCount(int32_t* count) const
{ *count = 1; return 0; }
XtFault 
JackDeviceList::GetId(int32_t index, char* buffer, int32_t* size) const
{ XtiCopyString("0", buffer, size); return 0; }
XtFault
JackDeviceList::GetName(char const* id, char* buffer, int32_t* size) const
{ XtiCopyString("JACK", buffer, size); return 0; }

XtFault
JackDeviceList::GetCapabilities(char const* id, XtDeviceCaps* capabilities) const
{ 
  auto result = XtDeviceCapsInput | XtDeviceCapsOutput;
  *capabilities = static_cast<XtDeviceCaps>(result);
  return ASE_OK; 
}

#endif // XT_ENABLE_JACK