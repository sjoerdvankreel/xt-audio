#ifndef XT_PRIVATE_BLOCKING_STREAM_HPP
#define XT_PRIVATE_BLOCKING_STREAM_HPP

#include <xt/api/private/StreamBase.hpp>
#include <xt/private/Shared.hpp>

#define XT_IMPLEMENT_BLOCKING_STREAM()  \
  void StopSlaveBuffer() override;      \
  void StopMasterBuffer() override;     \
  XtFault StartSlaveBuffer() override;  \
  XtFault StartMasterBuffer() override; \
  XtFault BlockMasterBuffer() override; \
  XtFault ProcessBuffer(bool prefill) override;

struct XtBlockingStream:
public XtStreamBase
{
  ~XtBlockingStream() { };
  XtBlockingStream() = default;

  virtual void StopSlaveBuffer() = 0;
  virtual void StopMasterBuffer() = 0;
  virtual XtFault StartSlaveBuffer() = 0;  
  virtual XtFault StartMasterBuffer() = 0;  
  virtual XtFault BlockMasterBuffer() = 0;
  virtual XtFault PrefillOutputBuffer() = 0;
  virtual XtFault ProcessBuffer(bool prefill) = 0;
};

#endif // XT_PRIVATE_BLOCKING_STREAM_HPP