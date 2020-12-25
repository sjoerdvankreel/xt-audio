#ifndef XT_BACKEND_DSOUND_SHARED_HPP
#define XT_BACKEND_DSOUND_SHARED_HPP
#if XT_ENABLE_DSOUND
#include <xt/private/Device.hpp>
#include <xt/private/Stream.hpp>
#include <xt/private/Service.hpp>
#include <xt/blocking/Stream.hpp>
#include <xt/blocking/Device.hpp>
#include <xt/private/DeviceList.hpp>
#include <xt/backend/dsound/Private.hpp>

#include <vector>
#include <atlbase.h>
#include <dsound.h>

struct DSoundService final:
public XtService 
{
  XT_IMPLEMENT_SERVICE(DSound);
};

struct DSoundDevice final:
public XtBlockingDevice
{
  DSoundDevice() = default;
  XT_IMPLEMENT_DEVICE();
  XT_IMPLEMENT_DEVICE_BLOCKING();
  XT_IMPLEMENT_DEVICE_BASE(DSound);

  GUID _id;
  CComPtr<IDirectSound> _output;
  CComPtr<IDirectSoundCapture> _input;
};

struct DSoundStream final:
public XtBlockingStream
{
  int32_t _frameSize;
  uint64_t _xtProcessed;
  uint64_t _dsProcessed;
  int32_t _bufferFrames;
  XtDsWaitableTimer _timer;
  int32_t _previousPosition;
  std::vector<uint8_t> _audio;
  CComPtr<IDirectSound> _output;
  CComPtr<IDirectSoundCapture> _input;
  CComPtr<IDirectSoundBuffer> _outputBuffer;
  CComPtr<IDirectSoundCaptureBuffer> _inputBuffer;
  
  DSoundStream() = default;
  XT_IMPLEMENT_STREAM_BASE();
  XT_IMPLEMENT_BLOCKING_STREAM();
  XT_IMPLEMENT_STREAM_BASE_SYSTEM(DSound);
};

struct DSoundDeviceList final:
public XtDeviceList
{
  DSoundDeviceList() = default;
  XT_IMPLEMENT_DEVICE_LIST(DSound);
  std::vector<XtDsDeviceInfo> _devices;
  
  HRESULT
  GetDeviceInfo(char const* id, XtDsDeviceInfo* device) const;
  static BOOL CALLBACK
  EnumCallback(GUID* id, wchar_t const* name, wchar_t const*, void* ctx);
};

#endif // XT_ENABLE_DSOUND
#endif // XT_BACKEND_DSOUND_SHARED_HPP