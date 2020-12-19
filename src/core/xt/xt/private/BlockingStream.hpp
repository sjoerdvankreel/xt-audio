#ifndef XT_PRIVATE_BLOCKING_STREAM_HPP
#define XT_PRIVATE_BLOCKING_STREAM_HPP

#include <xt/api/public/XtStream.h>
#include <xt/api/private/StreamBase.hpp>
#include <xt/private/Shared.hpp>

#define XT_IMPLEMENT_BLOCKING_STREAM()  \
  void StopSlaveBuffer() override;      \
  void StopMasterBuffer() override;     \
  XtFault ProcessBuffer() override;     \
  XtFault StartSlaveBuffer() override;  \
  XtFault StartMasterBuffer() override; \
  XtFault BlockMasterBuffer() override; \
  XtFault PrefillOutputBuffer() override; 

struct XtBlockingStream:
public XtStreamBase
{
  XtStream* _adapter;
  ~XtBlockingStream() { };
  XtBlockingStream() = default;

  void StopBuffer();
  XtFault StartBuffer();

  virtual void StopSlaveBuffer() = 0;
  virtual void StopMasterBuffer() = 0;
  virtual XtFault ProcessBuffer() = 0;
  virtual XtFault StartSlaveBuffer() = 0;  
  virtual XtFault StartMasterBuffer() = 0;  
  virtual XtFault BlockMasterBuffer() = 0;
  virtual XtFault PrefillOutputBuffer() = 0;
};

#endif // XT_PRIVATE_BLOCKING_STREAM_HPP