#ifndef XT_PRIVATE_BLOCKING_STREAM_HPP
#define XT_PRIVATE_BLOCKING_STREAM_HPP

#include <xt/api/private/Stream.hpp>
#include <xt/private/BlockingStreamLinux.hpp>
#include <xt/private/BlockingStreamWin32.hpp>
#include <cstdint>

enum class XtBlockingStreamState
{ Stopped, Starting, Started, Stopping, Closing, Closed };

struct XtBlockingStream:
public XtStream 
{
  int32_t _index;
  bool _aggregated;
  bool const _secondary;  
  XtBlockingStreamImpl _impl;
  XtBlockingStreamState _state;

  XT_IMPLEMENT_STREAM();
  XtBlockingStream(bool secondary):
  _index(-1),
  _aggregated(false),
  _secondary(secondary),
  _impl(secondary),
  _state(XtBlockingStreamState::Stopped) {}

  virtual void StopStream() = 0;
  virtual void StartStream() = 0;  
  virtual void OnXRun() const override;
  virtual void ProcessBuffer(bool prefill) = 0;

  void RequestStop();
  void ReceiveControl(XtBlockingStreamState state);
  void SendControl(XtBlockingStreamState from, XtBlockingStreamState to);
  bool VerifyOnBuffer(XtLocation const& location, XtFault fault, char const* expr);
};

#endif // XT_PRIVATE_BLOCKING_STREAM_HPP