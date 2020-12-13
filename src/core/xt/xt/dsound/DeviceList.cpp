#if XT_ENABLE_DSOUND
#include <xt/dsound/Shared.hpp>
#include <xt/dsound/Private.hpp>
#include <xt/private/Win32.hpp>

XtFault
DSoundDeviceList::GetCount(int32_t* count) const
{ *count = static_cast<int32_t>(_devices.size()); return DS_OK; }

XtFault 
DSoundDeviceList::GetId(int32_t index, char* buffer, int32_t* size) const
{ 
  XtiCopyString(_devices[index].id.c_str(), buffer, size);
  return DS_OK;
}

XtFault
DSoundDeviceList::GetName(char const* id, char* buffer, int32_t* size) const
{
  XtFault fault;
  XtDsDeviceInfo info;
  if((fault = GetDeviceInfo(id, &info)) != DS_OK) return fault;
  XtiCopyString(info.id.c_str(), buffer, size);
  return DS_OK;
}

XtFault
DSoundDeviceList::GetDeviceInfo(char const* id, XtDsDeviceInfo* device) const
{
  for(size_t i = 0; i < _devices.size(); i++)
    if(_devices[i].id == id) return *device = _devices[i], DS_OK;
  return DSERR_NODRIVER;
}

BOOL CALLBACK
DSoundDeviceList::EnumCallback(GUID* id, wchar_t const* name, wchar_t const*, void* ctx)
{
  XtDsDeviceInfo device;
  if(id == nullptr) return TRUE;
  auto devices = static_cast<std::vector<XtDsDeviceInfo>*>(ctx);
  device.id = XtiClassIdToUtf8(*id);
  device.name = XtiWideStringToUtf8(name);
  return TRUE;
}

#endif // XT_ENABLE_DSOUND