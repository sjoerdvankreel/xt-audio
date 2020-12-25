#ifndef XT_BACKEND_WASAPI_SHARED_HPP
#define XT_BACKEND_WASAPI_SHARED_HPP
#if XT_ENABLE_WASAPI

#include <xt/private/Device.hpp>
#include <xt/private/Stream.hpp>
#include <xt/private/Service.hpp>
#include <xt/blocking/Device.hpp>
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

struct WasapiDevice:
public XtBlockingDevice
{
  XtWasapiType _type;
  CComPtr<IAudioClient> _client;
  WasapiDevice() = default;
  XT_IMPLEMENT_DEVICE_BASE(WASAPI);
};

struct WasapiSharedDevice:
public WasapiDevice
{
  XT_IMPLEMENT_DEVICE();
  XT_IMPLEMENT_DEVICE_BLOCKING();
  WasapiSharedDevice() = default;
  CComPtr<IAudioClient3> _client3;
};

struct WasapiExclusiveDevice:
public WasapiDevice
{
  XT_IMPLEMENT_DEVICE();
  XT_IMPLEMENT_DEVICE_BLOCKING();
  WasapiExclusiveDevice() = default;
};

#endif // XT_ENABLE_WASAPI
#endif // XT_BACKEND_WASAPI_SHARED_HPP