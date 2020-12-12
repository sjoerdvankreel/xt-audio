#ifndef XT_DSOUND_SHARED_HPP
#define XT_DSOUND_SHARED_HPP
#if XT_ENABLE_DSOUND
#include <xt/api/private/Device.hpp>
#include <xt/api/private/Stream.hpp>
#include <xt/api/private/Service.hpp>
#include <xt/api/private/DeviceList.hpp>
#include <xt/dsound/Private.hpp>
#include <vector>

struct DSoundService:
public XtService 
{
  XT_IMPLEMENT_SERVICE(DSound);
};

struct DSoundDeviceList:
public XtDeviceList
{
  std::vector<XtDSDeviceInfo> 
  XT_IMPLEMENT_DEVICE_LIST(DSound);
};

#endif // XT_ENABLE_DSOUND
#endif // XT_DSOUND_SHARED_HPP