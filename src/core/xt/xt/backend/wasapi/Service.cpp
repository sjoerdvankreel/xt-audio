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
      XT_VERIFY_COM(XtiGetWasapiDeviceInfo(device, XtWasapiType::SharedCapture, &info));
      result->_devices.push_back(info);
      info.type = XtWasapiType::ExclusiveCapture;
      result->_devices.push_back(info);
    }
  }

  XT_VERIFY_COM(outputs->GetCount(&count));
  for(UINT i = 0; i < count; i++)
  {
    CComPtr<IMMDevice> device;
    XT_VERIFY_COM(outputs->Item(i, &device));
    XT_VERIFY_COM(XtiGetWasapiDeviceInfo(device, XtWasapiType::SharedRender, &info));
    if(output)
    {
      result->_devices.push_back(info);
      info.type = XtWasapiType::ExclusiveRender;
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

  auto flow = output ? eRender : eCapture;
  XT_VERIFY_COM(enumerator.CoCreateInstance(__uuidof(MMDeviceEnumerator)));
  hr = enumerator->GetDefaultAudioEndpoint(flow, eMultimedia, &device);
  if (hr == E_NOTFOUND) return S_OK;
  auto type = output? XtWasapiType::SharedRender: XtWasapiType::SharedCapture;
  XT_VERIFY_COM(XtiGetWasapiDeviceInfo(device, type, &info));
  XtiCopyString(XtiGetWasapiDeviceId(info).c_str(), buffer, size);
  *valid = XtTrue;
  return S_OK;
}

XtFault
WasapiService::OpenDevice(char const* id, XtDevice** device) const
{
  HRESULT hr;
  CComPtr<IMMDevice> d;
  CComPtr<IAudioClient> client;
  CComPtr<IAudioClient3> client3;
  CComPtr<IMMDeviceEnumerator> enumerator;

  auto info = XtiParseWasapiDeviceInfo(id);   
  auto wideId = XtiUtf8ToWideString(info.id.c_str());
  XT_VERIFY_COM(enumerator.CoCreateInstance(__uuidof(MMDeviceEnumerator)));
  XT_VERIFY_COM(enumerator->GetDevice(wideId.c_str(), &d));
  XT_VERIFY_COM(d->Activate(__uuidof(IAudioClient), CLSCTX_ALL, nullptr, reinterpret_cast<void**>(&client)));
  
  std::unique_ptr<WasapiDevice> result;
  if(XtiWasapiTypeIsExclusive(info.type)) result = std::make_unique<WasapiExclusiveDevice>();
  else
  {
    auto shared = new WasapiSharedDevice();
    result.reset(shared);
    hr = client.QueryInterface(&client3);
    if(hr != E_NOINTERFACE) XT_VERIFY_COM(hr);
    shared->_client3 = client3;
  }

  result->_device = d;
  result->_client = client;
  result->_type = info.type;
  *device = result.release();
  return S_OK;
}

#endif // XT_ENABLE_WASAPI