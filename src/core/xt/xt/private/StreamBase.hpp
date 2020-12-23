#ifndef XT_PRIVATE_STREAM_BASE_HPP
#define XT_PRIVATE_STREAM_BASE_HPP

#include <xt/api/Shared.h>
#include <xt/api/Structs.h>
#include <xt/shared/Shared.hpp>
#include <xt/shared/Structs.hpp>

#define XT_IMPLEMENT_STREAM_BASE()                         \
  void* GetHandle() override final;                        \
  XtFault GetFrames(int32_t* frames) const override final; \
  XtFault GetLatency(XtLatency* latency) const override final 
#define XT_IMPLEMENT_STREAM_BASE_SYSTEM(s) \
  XtSystem GetSystem() const override final { return XtSystem##s; }

struct XtStreamBase
{
  XtStreamBase() = default;
  virtual ~XtStreamBase() { };

  virtual void* GetHandle() = 0;
  virtual XtSystem GetSystem() const = 0;
  virtual void OnXRun(int32_t index) const = 0;
  virtual XtFault GetFrames(int32_t* frames) const = 0;
  virtual XtFault GetLatency(XtLatency* latency) const = 0;
  virtual XtFault OnBuffer(int32_t index, XtBuffer const* buffer) = 0;
};

#endif // XT_PRIVATE_STREAM_BASE_HPP