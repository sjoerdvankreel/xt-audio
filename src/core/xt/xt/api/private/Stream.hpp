#ifndef XT_API_PRIVATE_STREAM_HPP
#define XT_API_PRIVATE_STREAM_HPP

#include <xt/api/public/Structs.h>
#include <xt/private/Shared.hpp>
#include <xt/private/Structs.hpp>

#define XT_IMPLEMENT_STREAM()                        \
  XtFault Stop() override;                           \
  XtFault Start() override;                          \
  XtSystem GetSystem() const override;               \
  XtFault GetFrames(int32_t* frames) const override; \
  XtFault GetLatency(XtLatency* latency) const override;

struct XtStream 
{
  void* _user;
  bool _emulated;
  XtIOBuffers _buffers;
  XtDeviceStreamParams _params;

  virtual void OnXRun() const;
  void OnBuffer(XtBuffer const* buffer);

  virtual ~XtStream() {};
  virtual XtFault Stop() = 0;
  virtual XtFault Start() = 0;
  virtual XtSystem GetSystem() const = 0;
  virtual XtFault GetFrames(int32_t* frames) const = 0;
  virtual XtFault GetLatency(XtLatency* latency) const = 0;
};

#endif // XT_API_PRIVATE_STREAM_HPP