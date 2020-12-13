#ifndef XT_DSOUND_SHARED_HPP
#define XT_DSOUND_SHARED_HPP
#if XT_ENABLE_DSOUND
#include <xt/api/private/Device.hpp>
#include <xt/api/private/Stream.hpp>
#include <xt/api/private/Service.hpp>
#include <xt/api/private/DeviceList.hpp>
#include <xt/dsound/Private.hpp>
#include <vector>
#include <dsound.h>

struct DSoundService:
public XtService 
{
  XT_IMPLEMENT_SERVICE(DSound);
};

struct DSoundDevice:
public XtDevice
{
  DSoundDevice() = default;
  XT_IMPLEMENT_DEVICE(DSound);

  CComPtr<IDirectSound> _output;
  CComPtr<IDirectSoundCapture> _input;
};

struct DSoundDeviceList:
public XtDeviceList
{
  DSoundDeviceList() = default;
  XT_IMPLEMENT_DEVICE_LIST(DSound);
  std::vector<XtDSDeviceInfo> _inputs;
  std::vector<XtDSDeviceInfo> _outputs;

  static BOOL CALLBACK
  EnumCallback(GUID* id, wchar_t const* name, wchar_t const*, void* ctx);
};

#endif // XT_ENABLE_DSOUND
#endif // XT_DSOUND_SHARED_HPP