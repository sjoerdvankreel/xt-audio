#if XT_ENABLE_ASIO
#include <xt/asio/Shared.hpp>
#include <xt/private/Win32.hpp>
#include <xt/api/private/Platform.hpp>

XtFault
AsioService::OpenDeviceList(XtEnumFlags flags, XtDeviceList** list) const
{ *list = new AsioDeviceList; return ASE_OK; }

XtCapabilities
AsioService::GetCapabilities() const
{
  auto result = XtCapabilitiesTime
    | XtCapabilitiesLatency
    | XtCapabilitiesFullDuplex
    | XtCapabilitiesChannelMask;
  return static_cast<XtCapabilities>(result);
}

XtFault
AsioService::OpenDevice(char const* id, XtDevice** device) const
{  
  HRESULT hr;
  CLSID classId;
  CComPtr<IASIO> asio;
  auto wideId = XtiUtf8ToWideString(id);
  XT_VERIFY_COM(CLSIDFromString(wideId.data(), &classId));
  XT_VERIFY_COM(CoCreateInstance(classId, nullptr, CLSCTX_ALL, classId, reinterpret_cast<void**>(&asio)));
  if(!asio->init(XtPlatform::instance->_window)) return ASE_NotPresent;
  *device = new AsioDevice(asio);
  return ASE_OK;
}

#endif // XT_ENABLE_ASIO