#ifndef XT_PRIVATE_STREAM_HPP
#define XT_PRIVATE_STREAM_HPP

#include <xt/private/StreamBase.hpp>

#define XT_IMPLEMENT_STREAM()     \
  void Stop() override final;     \
  XtFault Start() override final; \
  XtBool IsRunning() const override final

struct XtStream:
public XtStreamBase
{
  void* _user;
  bool _emulated;
  XtIOBuffers _buffers;
  XtDeviceStreamParams _params;

  virtual void Stop() = 0;
  virtual XtFault Start() = 0;
  virtual XtBool IsRunning() const = 0;

  XtStream() = default;  
  void OnRunning(XtBool running) const;
  void OnXRun(int32_t index) const override final;
  uint32_t OnBuffer(int32_t index, XtBuffer const* buffer) override final;
};

#endif // XT_PRIVATE_STREAM_HPP