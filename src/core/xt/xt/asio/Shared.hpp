#ifndef XT_ASIO_SHARED_HPP
#define XT_ASIO_SHARED_HPP
#if XT_ENABLE_ASIO

#include <xt/api/private/Device.hpp>
#include <xt/api/private/Service.hpp>
#include <xt/api/private/DeviceList.hpp>
#include <atlbase.h>
#include <common/iasiodrv.h>
#include <host/pc/asiolist.h>

struct AsioService:
public XtService 
{
  XT_IMPLEMENT_SERVICE(ASIO);
};

struct AsioDeviceList:
public XtDeviceList
{
  AsioDeviceList() = default;
  XT_IMPLEMENT_DEVICE_LIST(ASIO);
  mutable AsioDriverList _drivers;
};

struct AsioDevice:
public XtDevice
{
  bool _streamOpen;
  CComPtr<IASIO> _asio;
  XT_IMPLEMENT_DEVICE(ASIO);
  AsioDevice(CComPtr<IASIO> asio);
};

#endif // XT_ENABLE_ASIO
#endif // XT_ASIO_SHARED_HPP