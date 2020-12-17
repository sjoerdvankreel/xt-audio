#ifndef XT_API_PRIVATE_STREAM_HPP
#define XT_API_PRIVATE_STREAM_HPP

#include <xt/api/private/StreamBase.hpp>

#define XT_IMPLEMENT_STREAM() \
  void Stop() override;       \
  XtFault Start() override;   \
  XtBool IsRunning() const override;         

struct XtStream:
public XtStreamBase
{
  XtStream() = default;
  void OnXRun(int32_t index) const;
  void OnRunning(XtBool running) const;
  uint32_t OnBuffer(XtBuffer const* buffer);

  virtual ~XtStream() {};
  virtual void Stop() = 0;
  virtual XtFault Start() = 0;
  virtual XtBool IsRunning() const = 0;
};

#endif // XT_API_PRIVATE_STREAM_HPP