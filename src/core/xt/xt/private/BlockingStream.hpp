#ifndef XT_PRIVATE_BLOCKING_STREAM_HPP
#define XT_PRIVATE_BLOCKING_STREAM_HPP

#include <xt/api/private/Stream.hpp>
#include <mutex>
#include <atomic>
#include <cstdint>
#include <condition_variable>

#define XT_IMPLEMENT_BLOCKING_STREAM()               \
  void StopStream() override;                        \
  void StartStream() override;                       \
  XtSystem GetSystem() const override;               \
  void ProcessBuffer(bool prefill) override;         \
  XtFault GetFrames(int32_t* frames) const override; \
  XtFault GetLatency(XtLatency* latency) const override;

#define XT_VERIFY_ON_BUFFER(expr) \
  VerifyOnBuffer({__FILE__,  __func__, __LINE__}, (expr), #expr)

enum class XtBlockingStreamState
{ Stopped, Starting, Started, Stopping, Closing, Closed };

struct XtBlockingStream:
public XtStream 
{
  int32_t _index;
  bool _aggregated;
  std::mutex _lock;
  bool const _secondary;
  std::condition_variable _control;
  std::condition_variable _respond;
  std::atomic<XtBlockingStreamState> _state;
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
  void ReceiveControl(XtBlockingStreamState state);
  void SendControl(XtBlockingStreamState from, XtBlockingStreamState to);
  bool VerifyOnBuffer(XtLocation const& location, XtFault fault, char const* expr);
};

#endif // XT_PRIVATE_BLOCKING_STREAM_HPP