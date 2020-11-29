#include <xt/private/BlockingStream.hpp>
#include <xt/private/Shared.hpp>
#include <xt/Private.hpp>

void 
XtBlockingStream::OnXRun() const
{
  auto xRun = _params.stream.onXRun;
  if(xRun == nullptr) return;
  if(!_aggregated) xRun(-1, _user);
  else xRun(_index, static_cast<XtAggregateContext*>(_user)->stream->_user);
}

XtFault
XtBlockingStream::Start() 
{
  if(_secondary) { ProcessBuffer(true); StartStream(); }
  else SendControl(XtBlockingStreamState::Starting, XtBlockingStreamState::Started);
  return 0;
}

XtFault
XtBlockingStream::Stop() {
  if(_secondary) StopStream();
  else SendControl(XtBlockingStreamState::Stopping, XtBlockingStreamState::Stopped);
  return 0;
}

bool
XtBlockingStream::VerifyOnBuffer(XtLocation const& location, XtFault fault, char const* expr)
{
  if(fault == 0) return true;
  XtBuffer buffer = { 0 };
  RequestStop();
  XtiTrace(location, expr);
  buffer.error = XtiCreateError(GetSystem(), fault);
  OnBuffer(&buffer);
  return false;
}

XtBlockingStream::~XtBlockingStream() 
{ if(!_secondary) SendControl(XtBlockingStreamState::Closing, XtBlockingStreamState::Closed); }