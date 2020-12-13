#if XT_ENABLE_DSOUND
#include <xt/dsound/Shared.hpp>
#include <xt/dsound/Private.hpp>
#include <xt/private/Win32.hpp>

XtFault
DSoundDeviceList::GetCount(int32_t* count) const
{ *count = static_cast<int32_t>(_inputs.size() + _outputs.size()); return DS_OK; }

XtFault 
DSoundDeviceList::GetId(int32_t index, char* buffer, int32_t* size) const
{ 
  if(index < _inputs.size()) XtiCopyString(_inputs[index].id.c_str(), buffer, size);
  else XtiCopyString(_outputs[index - _inputs.size()].id.c_str(), buffer, size);
  return DS_OK;
}

XtFault
DSoundDeviceList::GetName(char const* id, char* buffer, int32_t* size) const
{
  for(size_t i = 0; i < _inputs.size(); i++)
    if(_inputs[i].id == id) XtiCopyString(_inputs[i].id.c_str(), buffer, size);
  for(size_t i = 0; i < _outputs.size(); i++)
    if(_outputs[i].id == id) XtiCopyString(_outputs[i].id.c_str(), buffer, size);
  return DS_OK;
}

BOOL CALLBACK
DSoundDeviceList::EnumCallback(GUID* id, wchar_t const* name, wchar_t const*, void* ctx)
{
  XtDSDeviceInfo device;
  if(id == nullptr) return TRUE;
  auto devices = static_cast<std::vector<XtDSDeviceInfo>*>(ctx);
  device.id = XtiClassIdToUtf8(*id);
  device.name = XtiWideStringToUtf8(name);
  return TRUE;
}

#endif // XT_ENABLE_DSOUND