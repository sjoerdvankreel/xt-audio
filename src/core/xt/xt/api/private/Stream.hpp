#ifndef XT_API_PRIVATE_STREAM_HPP
#define XT_API_PRIVATE_STREAM_HPP

#include <xt/api/private/StreamBase.hpp>

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

  XtStream() = default;
  XT_IMPLEMENT_STREAM_BASE_CALLBACKS();

  virtual void Stop() = 0;
  virtual XtFault Start() = 0;
  virtual XtBool IsRunning() const = 0;
  void OnRunning(XtBool running) const;
};

#endif // XT_API_PRIVATE_STREAM_HPP