#ifndef XT_API_PRIVATE_STREAM_HPP
#define XT_API_PRIVATE_STREAM_HPP

#include <xt/api/public/Structs.h>
#include <xt/private/Structs.hpp>
#include <xt/private/Shared.hpp>

#define XT_IMPLEMENT_STREAM()                        \
  XtFault Stop() override;                           \
  XtFault Start() override;                          \
  XtBool IsRunning() const override;                 \
  XtFault GetFrames(int32_t* frames) const override; \
  XtFault GetLatency(XtLatency* latency) const override;

#define XT_IMLEMENT_STREAM_SYSTEM(s) \
  XtSystem GetSystem() const override { return XtSystem##s; }

struct XtStream 
{
  void* _user;
  bool _emulated;
  XtIOBuffers _buffers;
  XtDeviceStreamParams _params;

  XtStream() = default;
  virtual void OnXRun() const;
  void OnRunning(XtBool running) const;
  void OnBuffer(XtBuffer const* buffer);

  virtual ~XtStream() {};
  virtual XtFault Stop() = 0;
  virtual XtFault Start() = 0;
  virtual XtBool IsRunning() const = 0;
  virtual XtSystem GetSystem() const = 0;
  virtual XtFault GetFrames(int32_t* frames) const = 0;
  virtual XtFault GetLatency(XtLatency* latency) const = 0;
};

#endif // XT_API_PRIVATE_STREAM_HPP