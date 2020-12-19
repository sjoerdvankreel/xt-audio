#ifndef XT_API_PRIVATE_STREAM_BASE_HPP
#define XT_API_PRIVATE_STREAM_BASE_HPP

#include <xt/api/public/Shared.h>
#include <xt/api/public/Structs.h>
#include <xt/private/Structs.hpp>
#include <xt/private/Shared.hpp>

#define XT_IMPLEMENT_STREAM_BASE_CALLBACKS() \
  void OnXRun(int32_t index) const override; \
  uint32_t OnBuffer(XtBuffer const* buffer) override;
#define XT_IMPLEMENT_STREAM_BASE()                         \
  XtFault GetFrames(int32_t* frames) const override final; \
  XtFault GetLatency(XtLatency* latency) const override final 
#define XT_IMPLEMENT_STREAM_BASE_SYSTEM(s) \
  XtSystem GetSystem() const override final { return XtSystem##s; }

struct XtStreamBase
{
  XtStreamBase() = default;
  virtual ~XtStreamBase() { };

  virtual XtSystem GetSystem() const = 0;
  virtual void OnXRun(int32_t index) const = 0;
  virtual XtFault GetFrames(int32_t* frames) const = 0;
  virtual uint32_t OnBuffer(XtBuffer const* buffer) = 0;
  virtual XtFault GetLatency(XtLatency* latency) const = 0;
};

#endif // XT_API_PRIVATE_STREAM_BASE_HPP