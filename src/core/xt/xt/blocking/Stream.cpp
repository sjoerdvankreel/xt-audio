#include <xt/private/Stream.hpp>
#include <xt/blocking/Stream.hpp>

void
XtBlockingStream::OnXRun(int32_t index) const
{ _runner->OnXRun(index); }
uint32_t
XtBlockingStream::OnBuffer(int32_t index, XtBuffer const* buffer)
{ return _runner->OnBuffer(index, buffer); }

void
XtBlockingStream::StopBuffer()
{
  StopSlaveBuffer();
  StopMasterBuffer();
}

XtFault
XtBlockingStream::StartBuffer()
{
  XtFault fault;
  if((fault = StartMasterBuffer()) != 0) return fault;
  auto masterGuard = XtiGuard([this] { StopMasterBuffer(); });
  if((fault = StartSlaveBuffer()) != 0) return fault;
  masterGuard.Commit();
  return 0;
}