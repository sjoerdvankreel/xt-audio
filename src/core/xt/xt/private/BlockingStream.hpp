#ifndef XT_PRIVATE_BLOCKING_STREAM_HPP
#define XT_PRIVATE_BLOCKING_STREAM_HPP

#include <xt/api/private/Stream.hpp>
#include <xt/private/Shared.hpp>
#include <mutex>
#include <atomic>
#include <cstdint>
#include <condition_variable>

#define XT_VERIFY_ON_BUFFER(expr) \
  VerifyOnBuffer(XT_LOCATION, (expr), #expr)

#define XT_IMPLEMENT_BLOCKING_STREAM()               \
  void StopStream() override;                        \
  void StartStream() override;                       \
  XtSystem GetSystem() const override;               \
  void ProcessBuffer(bool prefill) override;         \
  XtFault GetFrames(int32_t* frames) const override; \
  XtFault GetLatency(XtLatency* latency) const override;

struct XtBlockingStream:
public XtStream 
{
  enum class State 
  { 
    Stopped, Starting, Started,
    Stopping, Closing, Closed 
  };

  int32_t _index;
  bool _aggregated;
  std::mutex _lock;
  bool const _secondary;
  std::atomic<State> _state;
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
  void ReceiveControl(State state);
  void SendControl(State from, State to);
  bool VerifyOnBuffer(XtLocation const& location, XtFault fault, char const* expr);
};

#endif // XT_PRIVATE_BLOCKING_STREAM_HPP