#ifndef XT_PRIVATE_BLOCKING_STREAM_HPP
#define XT_PRIVATE_BLOCKING_STREAM_HPP

#include <xt/api/private/Stream.hpp>
#include <xt/private/Enums.hpp>
#include <cstdint>

template <class TSelf>
struct XtBlockingStreamBase:
public XtStream 
{
  int32_t _index;
  bool _aggregated;
  bool const _secondary;
  XtBlockingStreamState _state;

  XT_IMPLEMENT_STREAM();
  ~XtBlockingStreamBase();
  XtBlockingStreamBase(bool secondary):
  _index(-1),
  _aggregated(false),
  _secondary(secondary),
  _state(XtBlockingStreamState::Stopped) {}

  virtual void StopStream() = 0;
  virtual void StartStream() = 0;  
  virtual void OnXRun() const override;
  virtual void ProcessBuffer(bool prefill) = 0;
  TSelf& self() { return *static_cast<TSelf*>(this); }

  void RequestStop();
  XtBlockingStreamState ReadState();
  void ReceiveControl(XtBlockingStreamState state);
  void SendControl(XtBlockingStreamState from, XtBlockingStreamState to);
  bool VerifyOnBuffer(XtLocation const& location, XtFault fault, char const* expr);
};

#endif // XT_PRIVATE_BLOCKING_STREAM_HPP