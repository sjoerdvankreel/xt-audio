#ifndef XT_BLOCKING_RUNNER_HPP
#define XT_BLOCKING_RUNNER_HPP

#include <xt/shared/Shared.hpp>
#include <xt/private/Stream.hpp>
#include <xt/blocking/Stream.hpp>

#include <mutex>
#include <memory>
#include <atomic>
#include <cstdint>
#include <condition_variable>

struct XtBlockingRunner:
public XtStream
{
  enum class State 
  { 
    Stopped, Starting, Started,
    Stopping, Closing, Closed 
  };

  bool _received;
  std::mutex _lock;
  std::atomic<State> _state;
  std::condition_variable _control;
  std::condition_variable _respond;
  std::unique_ptr<XtBlockingStream> _stream;
  static inline int const WaitTimeoutMs = 10000;

  XT_IMPLEMENT_STREAM();
  XT_IMPLEMENT_STREAM_BASE();
  XtSystem GetSystem() const override final;
  ~XtBlockingRunner();
  XtBlockingRunner(XtBlockingStream* stream);
  
  void SendControl(State from);
  void ReceiveControl(State state);
  static void RunBlockingStream(XtBlockingRunner* runner);
};

#endif // XT_BLOCKING_RUNNER_HPP