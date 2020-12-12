#ifndef XT_ALSA_SHARED_HPP
#define XT_ALSA_SHARED_HPP
#if XT_ENABLE_ALSA
#include <xt/api/private/Device.hpp>
#include <xt/api/private/Stream.hpp>
#include <xt/api/private/Service.hpp>
#include <xt/api/private/DeviceList.hpp>

struct AlsaDeviceList:
public XtDeviceList
{
  void** const _hints;
  ~AlsaDeviceList();
  AlsaDeviceList(void** hints);
  XT_IMPLEMENT_DEVICE_LIST(ALSA);
};

#endif // XT_ENABLE_ALSA
#endif // XT_ALSA_SHARED_HPP