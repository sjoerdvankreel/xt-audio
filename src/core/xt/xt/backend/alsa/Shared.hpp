#ifndef XT_BACKEND_ALSA_SHARED_HPP
#define XT_BACKEND_ALSA_SHARED_HPP
#if XT_ENABLE_ALSA
#include <xt/private/Device.hpp>
#include <xt/private/Stream.hpp>
#include <xt/private/Service.hpp>
#include <xt/private/DeviceList.hpp>

#include <alsa/asoundlib.h>
#include <cstdint>

struct AlsaService final:
public XtService
{
  XT_IMPLEMENT_SERVICE(ALSA);
};

struct AlsaDeviceList final:
public XtDeviceList
{
  void** _hints;
  int32_t _count;
  ~AlsaDeviceList();
  AlsaDeviceList() = default;
  XT_IMPLEMENT_DEVICE_LIST(ALSA);
};

#endif // XT_ENABLE_ALSA
#endif // XT_BACKEND_ALSA_SHARED_HPP