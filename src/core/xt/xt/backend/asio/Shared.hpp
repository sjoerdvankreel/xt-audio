#ifndef XT_BACKEND_ASIO_SHARED_HPP
#define XT_BACKEND_ASIO_SHARED_HPP
#if XT_ENABLE_ASIO

#define NOMINMAX 1
#include <xt/shared/Shared.hpp>
#include <xt/private/Device.hpp>
#include <xt/private/Stream.hpp>
#include <xt/private/Service.hpp>
#include <xt/private/DeviceList.hpp>

#include <atlbase.h>
#include <asmjit/asmjit.h>
#include <common/iasiodrv.h>
#include <host/pc/asiolist.h>
#include <memory>

#define XT_ASIO_CALL __cdecl

struct AsioService final:
public XtService 
{
  XT_IMPLEMENT_SERVICE(ASIO);
};

struct AsioDeviceList final:
public XtDeviceList
{
  AsioDeviceList() = default;
  XT_IMPLEMENT_DEVICE_LIST(ASIO);
  mutable AsioDriverList _drivers;
};

struct AsioDevice final:
public XtDevice
{
  CComPtr<IASIO> _asio;
  XT_IMPLEMENT_DEVICE(ASIO);
  XT_IMPLEMENT_DEVICE_STREAM();
  AsioDevice(CComPtr<IASIO> asio);
};

struct AsioStream final:
public XtStream
{
  long _bufferSize;
  CComPtr<IASIO> _asio;
  bool _issueOutputReady;
  ASIOCallbacks _callbacks;
  std::atomic_int _running;
  std::vector<void*> _inputs;
  std::vector<void*> _outputs;
  std::atomic_int _insideCallback;
  std::vector<ASIOBufferInfo> _buffers;
  std::unique_ptr<asmjit::JitRuntime> _runtime;

  ~AsioStream();
  AsioStream() = default;
  XT_IMPLEMENT_STREAM();
  XT_IMPLEMENT_STREAM_BASE();
  XT_IMPLEMENT_STREAM_BASE_SYSTEM(ASIO);

  static void XT_ASIO_CALL
  BufferSwitch(long index, ASIOBool direct, void* ctx);
  static ASIOTime* XT_ASIO_CALL
  BufferSwitchTimeInfo(ASIOTime* time, long index, ASIOBool direct, void* ctx);
};

#endif // XT_ENABLE_ASIO
#endif // XT_BACKEND_ASIO_SHARED_HPP