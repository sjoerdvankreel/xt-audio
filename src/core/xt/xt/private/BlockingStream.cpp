#include <xt/private/BlockingStream.hpp>
#include <xt/api/private/Stream.hpp>

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