#include <xt/private/BlockingStream.hpp>

XtStream const*
XtBlockingStream::GetStream() const
{ return _stream; }

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