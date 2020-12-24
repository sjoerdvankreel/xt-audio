#ifndef XT_BACKEND_WASAPI_SHARED_HPP
#define XT_BACKEND_WASAPI_SHARED_HPP
#if XT_ENABLE_WASAPI

#include <xt/private/Device.hpp>
#include <xt/private/Stream.hpp>
#include <xt/private/Service.hpp>
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

#endif // XT_ENABLE_WASAPI
#endif // XT_BACKEND_WASAPI_SHARED_HPP