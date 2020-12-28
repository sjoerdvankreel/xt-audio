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
WasapiDeviceList::GetCapabilities(char const* id, XtDeviceCaps* capabilities) const
{
  XtWasapiDeviceInfo info;
  if(!XtiParseWasapiDeviceInfo(std::string(id), &info)) return AUDCLNT_E_DEVICE_INVALIDATED;
  *capabilities = static_cast<XtDeviceCaps>(XtiGetWasapiDeviceCaps(info.type));
  return S_OK;
}

XtFault
WasapiDeviceList::GetName(char const* id, char* buffer, int32_t* size) const
{
  HRESULT hr;
  PROPVARIANT pv;
  std::ostringstream oss;
  XtWasapiDeviceInfo info;
  CComPtr<IMMDevice> device;
  CComPtr<IPropertyStore> store;
  CComPtr<IMMDeviceEnumerator> enumerator;

  PropVariantInit(&pv);
  if(!XtiParseWasapiDeviceInfo(std::string(id), &info)) return AUDCLNT_E_DEVICE_INVALIDATED;
  std::wstring wideId = XtiUtf8ToWideString(info.id.c_str());
  XT_VERIFY_COM(enumerator.CoCreateInstance(__uuidof(MMDeviceEnumerator)));
  XT_VERIFY_COM(enumerator->GetDevice(wideId.c_str(), &device));
  XT_VERIFY_COM(device->OpenPropertyStore(STGM_READ, &store));
  XT_VERIFY_COM(store->GetValue(PKEY_Device_FriendlyName, &pv));
  std::string name = XtiWideStringToUtf8(pv.pwszVal);
  PropVariantClear(&pv);
  oss << name.c_str() << " (" << XtiGetWasapiNameSuffix(info.type) << ")";
  XtiCopyString(oss.str().c_str(), buffer, size);
  return S_OK;
}

#endif // XT_ENABLE_WASAPI