#ifndef XT_BACKEND_ALSA_SHARED_HPP
#define XT_BACKEND_ALSA_SHARED_HPP
#if XT_ENABLE_ALSA
#include <xt/private/Device.hpp>
#include <xt/private/Stream.hpp>
#include <xt/private/Service.hpp>
#include <xt/blocking/Stream.hpp>
#include <xt/blocking/Device.hpp>
#include <xt/private/DeviceList.hpp>
#include <xt/backend/alsa/Private.hpp>

#include <alsa/asoundlib.h>
#include <vector>
#include <cstdint>

struct AlsaService final:
public XtService
{
  AlsaService();
  ~AlsaService();
  XT_IMPLEMENT_SERVICE(ALSA);
};

struct AlsaDevice final:
public XtBlockingDevice
{
  XtAlsaDeviceInfo _info;
  AlsaDevice() = default;
  
  XT_IMPLEMENT_DEVICE();
  XT_IMPLEMENT_DEVICE_BLOCKING();
  XT_IMPLEMENT_DEVICE_BASE(ALSA);  
};

struct AlsaDeviceList final:
public XtDeviceList
{
  AlsaDeviceList() = default;
  XT_IMPLEMENT_DEVICE_LIST(ALSA);
  std::vector<XtAlsaDeviceInfo> _devices;
};

#endif // XT_ENABLE_ALSA
#endif // XT_BACKEND_ALSA_SHARED_HPP