#ifndef XT_API_PRIVATE_STREAM_HPP
#define XT_API_PRIVATE_STREAM_HPP

#include <xt/api/private/StreamBase.hpp>

#define XT_IMPLEMENT_STREAM() \
  void Stop() override;       \
  XtFault Start() override;   \
  XtBool IsRunning() const override;
#define XT_IMPLEMENT_STREAM_SELF() \
  XtStream const* GetStream() const override { return this; }

struct XtStream:
public XtStreamBase
{
  XtStream() = default;
  void OnRunning(XtBool running) const;

  virtual void Stop() = 0;
  virtual XtFault Start() = 0;
  virtual XtBool IsRunning() const = 0;
};

#endif // XT_API_PRIVATE_STREAM_HPP