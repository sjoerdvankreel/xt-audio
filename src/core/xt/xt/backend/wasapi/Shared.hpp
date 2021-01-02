#ifndef XT_BACKEND_WASAPI_SHARED_HPP
#define XT_BACKEND_WASAPI_SHARED_HPP
#if XT_ENABLE_WASAPI

#include <xt/private/Device.hpp>
#include <xt/private/Stream.hpp>
#include <xt/private/Service.hpp>
#include <xt/blocking/Device.hpp>
#include <xt/blocking/Stream.hpp>
#include <xt/private/DeviceList.hpp>
#include <xt/backend/wasapi/Private.hpp>

#include <atlbase.h>
#include <mmdeviceapi.h>
#include <Audioclient.h>
#include <vector>

struct WasapiService final:
public XtService
{
  XT_IMPLEMENT_SERVICE(WASAPI);
};

struct WasapiDeviceList final:
public XtDeviceList
{
  WasapiDeviceList() = default;
  XT_IMPLEMENT_DEVICE_LIST(WASAPI);
  std::vector<XtWasapiDeviceInfo> _devices;
};

struct WasapiStream final:
public XtBlockingStream
{
  UINT _frames;
  XtWsEvent _event;
  XtWasapiType _type;
  HANDLE _mmcssHandle;
  CComPtr<IAudioClock> _clock;
  CComPtr<IAudioClock2> _clock2;
  CComPtr<IAudioClient> _client;
  CComPtr<IAudioClient> _loopback;
  CComPtr<IAudioRenderClient> _render;
  CComPtr<IAudioCaptureClient> _capture;

  WasapiStream() = default;  
  XT_IMPLEMENT_STREAM_BASE();
  XT_IMPLEMENT_BLOCKING_STREAM();
  XT_IMPLEMENT_STREAM_BASE_SYSTEM(WASAPI);
};

struct WasapiDevice:
public XtBlockingDevice
{
  XtWasapiType _type;
  CComPtr<IMMDevice> _device;
  CComPtr<IAudioClient> _client;
  WasapiDevice() = default;  

  XT_IMPLEMENT_DEVICE_BLOCKING();
  XT_IMPLEMENT_DEVICE_BASE(WASAPI);
  virtual HRESULT InitializeStream(XtBlockingParams const* params, 
    REFERENCE_TIME buffer, WasapiStream* stream) = 0;
};

struct WasapiSharedDevice final:
public WasapiDevice
{
  WasapiSharedDevice() = default;
  CComPtr<IAudioClient3> _client3;

  XT_IMPLEMENT_DEVICE();
  HRESULT InitializeStream(XtBlockingParams const* params,
    REFERENCE_TIME buffer, WasapiStream* stream) override final;
};

struct WasapiExclusiveDevice final:
public WasapiDevice
{
  XT_IMPLEMENT_DEVICE();
  WasapiExclusiveDevice() = default;
  HRESULT InitializeStream(XtBlockingParams const* params,
    REFERENCE_TIME buffer, WasapiStream* stream) override final;
};

#endif // XT_ENABLE_WASAPI
#endif // XT_BACKEND_WASAPI_SHARED_HPP