#include <xt/private/BlockingStream.hpp>
#include <xt/private/BlockingStreamWin32.hpp>
#include <xt/private/BlockingStreamLinux.hpp>
#include <xt/private/Shared.hpp>
#include <xt/Private.hpp>

template <class TSelf>
XtBlockingStreamBase<TSelf>::
~XtBlockingStreamBase() 
{ if(!_secondary) SendControl(XtBlockingStreamState::Closing, XtBlockingStreamState::Closed); }

template <class TSelf> bool
XtBlockingStreamBase<TSelf>::
VerifyOnBuffer(XtLocation const& location, XtFault fault, char const* expr)
{
  if(fault == 0) return true;
  XtBuffer buffer = { 0 };
  RequestStop();
  XtiTrace(location, expr);
  buffer.error = XtiCreateError(GetSystem(), fault);
  OnBuffer(&buffer);
  return false;
}

template <class TSelf> void 
XtBlockingStreamBase<TSelf>::
OnXRun() const
{
  auto xRun = _params.stream.onXRun;
  if(xRun == nullptr) return;
  if(!_aggregated) xRun(-1, _user);
  else xRun(_index, static_cast<XtAggregateContext*>(_user)->stream->_user);
}

template <class TSelf> XtFault
XtBlockingStreamBase<TSelf>::
Start() 
{
  if(_secondary) { ProcessBuffer(true); StartStream(); }
  else SendControl(XtBlockingStreamState::Starting, XtBlockingStreamState::Started);
  return 0;
}

template <class TSelf> XtFault
XtBlockingStreamBase<TSelf>::
Stop() 
{
  if(_secondary) StopStream();
  else SendControl(XtBlockingStreamState::Stopping, XtBlockingStreamState::Stopped);
  return 0;
}