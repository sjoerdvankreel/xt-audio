#if XT_ENABLE_WASAPI
#include <xt/shared/Win32.hpp>
#include <xt/backend/wasapi/Shared.hpp>
#include <xt/backend/wasapi/Private.hpp>

#include <functiondiscoverykeys_devpkey.h>
#include <sstream>

XtFault
WasapiDeviceList::GetCount(int32_t* count) const
{ *count = static_cast<int32_t>(_devices.size()); return S_OK; }
  
XtFault 
WasapiDeviceList::GetId(int32_t index, char* buffer, int32_t* size) const
{
  std::string id = XtiGetWasapiDeviceId(_devices[index]);
  XtiCopyString(id.c_str(), buffer, size);
  return S_OK;
}

XtFault
WasapiDeviceList::GetName(char const* id, char* buffer, int32_t* size) const
{
  HRESULT hr;
  PROPVARIANT pv;
  char const* type;
  std::ostringstream oss;
  CComPtr<IMMDevice> device;
  CComPtr<IPropertyStore> store;
  CComPtr<IMMDeviceEnumerator> enumerator;

  PropVariantInit(&pv);
  XtWasapiDeviceInfo info = XtiParseWasapiDeviceInfo(std::string(id));
  std::wstring wideId = XtiUtf8ToWideString(info.id.c_str());
  XT_VERIFY_COM(enumerator.CoCreateInstance(__uuidof(MMDeviceEnumerator)));
  XT_VERIFY_COM(enumerator->GetDevice(wideId.c_str(), &device));
  XT_VERIFY_COM(device->OpenPropertyStore(STGM_READ, &store));
  XT_VERIFY_COM(store->GetValue(PKEY_Device_FriendlyName, &pv));
  std::string name = XtiWideStringToUtf8(pv.pwszVal);
  PropVariantClear(&pv);
    
  switch(info.type)
  {
  case XtWasapiType::Shared: type = "Shared"; break;
  case XtWasapiType::Loopback: type = "Loopback"; break;
  case XtWasapiType::Exclusive: type = "Exclusive"; break;
  default: XT_ASSERT(false);
  }
  oss << name.c_str() << " (" << type << ")";
  XtiCopyString(oss.str().c_str(), buffer, size);
  return S_OK;
}

#endif // XT_ENABLE_WASAPI