#if XT_ENABLE_ASIO
#include <xt/shared/Win32.hpp>
#include <xt/backend/asio/Shared.hpp>
#include <xt/backend/asio/Private.hpp>

void*
AsioDeviceList::GetHandle() const
{ return &_drivers; }

XtFault
AsioDeviceList::GetCount(int32_t* count) const
{
  *count = _drivers.asioGetNumDev();
  return ASE_OK;
}
  
XtFault 
AsioDeviceList::GetId(int32_t index, char* buffer, int32_t* size) const
{  
  HRESULT hr;
  CLSID classId;
  LPOLESTR wide;
  XT_VERIFY_ASIO(_drivers.asioGetDriverCLSID(index, &classId));
  XT_VERIFY_COM(StringFromCLSID(classId, &wide));
  std::string utf8 = XtiWideStringToUtf8(wide);
  CoTaskMemFree(wide);
  XtiCopyString(utf8.c_str(), buffer, size);
  return ASE_OK;
}

XtFault
AsioDeviceList::GetName(char const* id, char* buffer, int32_t* size) const
{
  CLSID current;
  LONG index = -1;
  CLSID classId = XtiUtf8ToClassId(id);
  std::string name(MAXDRVNAMELEN + 1, '\0');
  for(LONG i = 0; i < _drivers.asioGetNumDev(); i++)
  {
    XT_VERIFY_ASIO(_drivers.asioGetDriverCLSID(i, &current));
    if(current != classId) continue;
    index = i;
    break;
  }
  if(index == -1) return static_cast<XtFault>(DRVERR_DEVICE_NOT_FOUND);
  XT_VERIFY_ASIO(_drivers.asioGetDriverName(index, name.data(), MAXDRVNAMELEN));
  XtiCopyString(name.c_str(), buffer, size);
  return ASE_OK;
}

#endif // XT_ENABLE_ASIO