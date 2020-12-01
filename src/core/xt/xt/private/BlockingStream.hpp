#ifndef XT_PRIVATE_BLOCKING_STREAM_HPP
#define XT_PRIVATE_BLOCKING_STREAM_HPP

#include <xt/api/private/Stream.hpp>
#include <mutex>
#include <cstdint>
#include <condition_variable>

enum class XtBlockingStreamState
{ Stopped, Starting, Started, Stopping, Closing, Closed };

#define XT_VERIFY_ON_BUFFER(expr) \
VerifyOnBuffer({__FILE__,  __func__, __LINE__}, (expr), #expr)

struct XtBlockingStream:
public XtStream 
{
  int32_t _index;
  bool _aggregated;
  std::mutex _lock;
  bool const _secondary;
  XtBlockingStreamState _state;
  std::condition_variable _control;
  std::condition_variable _respond;
  static inline int const WaitTimeoutMs = 10000;

  ~XtBlockingStream();
  XtBlockingStream(bool secondary);

  virtual XtFault Stop() override final;
  virtual XtFault Start() override final;
  virtual void OnXRun() const override final;

  virtual void StopStream() = 0;
  virtual void StartStream() = 0;  
  virtual void ProcessBuffer(bool prefill) = 0;
  static void OnBlockingBuffer(XtBlockingStream* stream);

  void RequestStop();
  XtBlockingStreamState ReadState();
  void ReceiveControl(XtBlockingStreamState state);
  void SendControl(XtBlockingStreamState from, XtBlockingStreamState to);
  bool VerifyOnBuffer(XtLocation const& location, XtFault fault, char const* expr);
};

#endif // XT_PRIVATE_BLOCKING_STREAM_HPP