#if XT_ENABLE_DSOUND
#include <xt/shared/Win32.hpp>
#include <xt/backend/dsound/Shared.hpp>
#include <xt/backend/dsound/Private.hpp>

void*
DSoundDeviceList::GetHandle() const
{ return nullptr; }
XtFault
DSoundDeviceList::GetCount(int32_t* count) const
{ *count = static_cast<int32_t>(_devices.size()); return DS_OK; }

XtFault 
DSoundDeviceList::GetId(int32_t index, char* buffer, int32_t* size) const
{ 
  auto id = XtiClassIdToUtf8(_devices[index].id);
  XtiCopyString(id.c_str(), buffer, size);
  return DS_OK;
}

XtFault
DSoundDeviceList::GetName(char const* id, char* buffer, int32_t* size) const
{
  XtFault fault;
  XtDsDeviceInfo info;
  if((fault = GetDeviceInfo(id, &info)) != DS_OK) return fault;
  XtiCopyString(info.name.c_str(), buffer, size);
  return DS_OK;
}

XtFault
DSoundDeviceList::GetDeviceInfo(char const* id, XtDsDeviceInfo* device) const
{
  for(size_t i = 0; i < _devices.size(); i++)
    if(!strcmp(XtiClassIdToUtf8(_devices[i].id).c_str(), id)) return *device = _devices[i], DS_OK;
  return DSERR_NODRIVER;
}

BOOL CALLBACK
DSoundDeviceList::EnumCallback(GUID* id, wchar_t const* name, wchar_t const*, void* ctx)
{
  XtDsDeviceInfo device;
  if(id == nullptr) return TRUE;
  auto devices = static_cast<std::vector<XtDsDeviceInfo>*>(ctx);
  device.id = *id;
  device.name = XtiWideStringToUtf8(name);
  devices->push_back(device);
  return TRUE;
}

#endif // XT_ENABLE_DSOUND