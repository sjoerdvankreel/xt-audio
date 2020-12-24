#if XT_ENABLE_WASAPI
#include <xt/shared/Win32.hpp>
#include <xt/backend/wasapi/Shared.hpp>
#include <xt/backend/wasapi/Private.hpp>

#include <string>
#include <memory>

XtFault
WasapiService::GetFormatFault() const
{ return AUDCLNT_E_UNSUPPORTED_FORMAT; }

XtServiceCaps
WasapiService::GetCapabilities() const
{
  auto result = XtServiceCapsTime
    | XtServiceCapsLatency
    | XtServiceCapsAggregation
    | XtServiceCapsChannelMask
    | XtServiceCapsXRunDetection;
  return static_cast<XtServiceCaps>(result);
}

XtFault
WasapiService::OpenDevice(char const* id, XtDevice** device) const
{
  return S_OK;
}

XtFault
WasapiService::OpenDeviceList(XtEnumFlags flags, XtDeviceList** list) const
{  
  UINT count;
  HRESULT hr;  
  XtWasapiDeviceInfo info;
  CComPtr<IMMDeviceCollection> inputs;
  CComPtr<IMMDeviceCollection> outputs;
  CComPtr<IMMDeviceEnumerator> enumerator;

  bool input = (flags & XtEnumFlagsInput) != 0;
  bool output = (flags & XtEnumFlagsOutput) != 0;
  auto result = std::make_unique<WasapiDeviceList>();
  XT_VERIFY_COM(enumerator.CoCreateInstance(__uuidof(MMDeviceEnumerator)));
  XT_VERIFY_COM(enumerator->EnumAudioEndpoints(eCapture, DEVICE_STATE_ACTIVE, &inputs));
  XT_VERIFY_COM(enumerator->EnumAudioEndpoints(eRender, DEVICE_STATE_ACTIVE, &outputs));

  if(input)
  {
    XT_VERIFY_COM(inputs->GetCount(&count));
    for(UINT i = 0; i < count; i++)
    {
      CComPtr<IMMDevice> device;
      XT_VERIFY_COM(inputs->Item(i, &device));
      XT_VERIFY_COM(XtiGetWasapiDeviceInfo(device, XtWasapiType::Shared, &info));
      result->_devices.push_back(info);
      info.type = XtWasapiType::Exclusive;
      result->_devices.push_back(info);
    }
  }

  XT_VERIFY_COM(outputs->GetCount(&count));
  for(UINT i = 0; i < count; i++)
  {
    CComPtr<IMMDevice> device;
    XT_VERIFY_COM(outputs->Item(i, &device));
    XT_VERIFY_COM(XtiGetWasapiDeviceInfo(device, XtWasapiType::Shared, &info));
    if(output)
    {
      result->_devices.push_back(info);
      info.type = XtWasapiType::Exclusive;
      result->_devices.push_back(info);
    }
    if(input)
    {
      info.type = XtWasapiType::Loopback;
      result->_devices.push_back(info);
    }
  }

  *list = result.release();
  return S_OK;
}

XtFault
WasapiService::GetDefaultDeviceId(XtBool output, XtBool* valid, char* buffer, int32_t* size) const
{
  HRESULT hr;
  XtWasapiDeviceInfo info;
  CComHeapPtr<wchar_t> id;
  CComPtr<IMMDevice> device;
  CComPtr<IMMDeviceEnumerator> enumerator;

  XT_VERIFY_COM(enumerator.CoCreateInstance(__uuidof(MMDeviceEnumerator)));
  hr = enumerator->GetDefaultAudioEndpoint(output ? eRender : eCapture, eMultimedia, &device);
  if (hr == E_NOTFOUND) return S_OK;
  XT_VERIFY_COM(XtiGetWasapiDeviceInfo(device, XtWasapiType::Shared, &info));
  XtiCopyString(XtiGetWasapiDeviceId(info).c_str(), buffer, size);
  *valid = XtTrue;
  return S_OK;
}

#endif // XT_ENABLE_WASAPI