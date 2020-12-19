#ifndef XT_API_PRIVATE_STREAM_BASE_HPP
#define XT_API_PRIVATE_STREAM_BASE_HPP

#include <xt/api/public/Shared.h>
#include <xt/api/public/Structs.h>
#include <xt/private/Structs.hpp>
#include <xt/private/Shared.hpp>

#define XT_IMPLEMENT_STREAM_BASE()                   \
  XtFault GetFrames(int32_t* frames) const override; \
  XtFault GetLatency(XtLatency* latency) const override; 
#define XT_IMPLEMENT_STREAM_BASE_SYSTEM(s) \
  XtSystem GetSystem() const override { return XtSystem##s; }

struct XtStreamBase
{
  void* _user;
  bool _emulated;
  XtIOBuffers _buffers;
  XtDeviceStreamParams _params;

  XtStreamBase() = default;
  void OnXRun(int32_t index) const;
  uint32_t OnBuffer(XtBuffer const* buffer);

  virtual ~XtStreamBase() { };
  virtual XtSystem GetSystem() const = 0;
  virtual XtStream const* GetStream() const = 0;
  virtual XtFault GetFrames(int32_t* frames) const = 0;
  virtual XtFault GetLatency(XtLatency* latency) const = 0;
};

#endif // XT_API_PRIVATE_STREAM_BASE_HPP