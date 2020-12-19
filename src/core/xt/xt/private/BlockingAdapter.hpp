#ifndef XT_PRIVATE_BLOCKING_ADAPTER_HPP
#define XT_PRIVATE_BLOCKING_ADAPTER_HPP

#include <xt/api/private/Stream.hpp>
#include <xt/private/BlockingStream.hpp>
#include <xt/private/Shared.hpp>
#include <mutex>
#include <memory>
#include <atomic>
#include <cstdint>
#include <condition_variable>

struct XtBlockingAdapter:
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
  ~XtBlockingAdapter();
  XtBlockingAdapter(XtBlockingStream* stream);
  
  void SendControl(State from);
  void ReceiveControl(State state);
  static void RunBlockingStream(XtBlockingAdapter* adapter);
};

#endif // XT_PRIVATE_BLOCKING_ADAPTER_HPP