#ifndef XT_BLOCKING_STREAM_HPP
#define XT_BLOCKING_STREAM_HPP

#include <xt/api/XtStream.h>
#include <xt/shared/Shared.hpp>
#include <xt/private/StreamBase.hpp>

#define XT_IMPLEMENT_BLOCKING_STREAM()        \
  void StopSlaveBuffer() override final;      \
  void StopMasterBuffer() override final;     \
  XtFault ProcessBuffer() override final;     \
  XtFault StartSlaveBuffer() override final;  \
  XtFault StartMasterBuffer() override final; \
  XtFault BlockMasterBuffer() override final; \
  XtFault PrefillOutputBuffer() override final 

struct XtBlockingStream:
public XtStreamBase
{
  XtStream* _runner;
  XtBlockingParams _params;

  ~XtBlockingStream() { };
  XtBlockingStream() = default;

  virtual void StopSlaveBuffer() = 0;
  virtual void StopMasterBuffer() = 0;
  virtual XtFault ProcessBuffer() = 0;
  virtual XtFault StartSlaveBuffer() = 0;  
  virtual XtFault StartMasterBuffer() = 0;  
  virtual XtFault BlockMasterBuffer() = 0;
  virtual XtFault PrefillOutputBuffer() = 0;

  void StopBuffer();
  XtFault StartBuffer();
  void OnXRun(int32_t index) const override final;
  uint32_t OnBuffer(int32_t index, XtBuffer const* buffer) override;
};

#endif // XT_BLOCKING_STREAM_HPP