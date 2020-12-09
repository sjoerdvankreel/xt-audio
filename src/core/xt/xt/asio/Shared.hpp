#ifndef XT_ASIO_SHARED_HPP
#define XT_ASIO_SHARED_HPP
#if XT_ENABLE_ASIO

#define NOMINMAX 1
#include <xt/private/Shared.hpp>
#include <xt/api/private/Device.hpp>
#include <xt/api/private/Stream.hpp>
#include <xt/api/private/Service.hpp>
#include <xt/api/private/DeviceList.hpp>
#include <atlbase.h>
#include <asmjit/asmjit.h>
#include <common/iasiodrv.h>
#include <host/pc/asiolist.h>
#include <memory>

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

struct AsioStream:
public XtStream
{
  CComPtr<IASIO> _asio;
  bool _issueOutputReady;
  long const _bufferSize;
  ASIOCallbacks _callbacks;
  std::atomic_int _running;
  std::vector<void*> _inputs;
  std::vector<void*> _outputs;
  std::atomic_int _insideCallback;
  std::vector<ASIOBufferInfo> _buffers;
  std::unique_ptr<asmjit::JitRuntime> _runtime;

  ~AsioStream();
  XT_IMPLEMENT_STREAM();
  XT_IMLEMENT_STREAM_SYSTEM(ASIO);
};

#endif // XT_ENABLE_ASIO
#endif // XT_ASIO_SHARED_HPP