#ifndef XT_PRIVATE_AGGREGATE_STREAM_HPP
#define XT_PRIVATE_AGGREGATE_STREAM_HPP

#include <xt/api/public/Structs.h>
#include <xt/api/private/Stream.hpp>
#include <xt/private/Structs.hpp>
#include <xt/private/RingBuffer.hpp>
#include <xt/private/BlockingStream.hpp>
#include <cstdint>
#include <vector>
#include <atomic>
#include <memory>

struct XtAggregateStream: 
public XtStream
{
  int32_t _frames;
  XtSystem _system;
  XtIOBuffers _weave;
  int32_t _masterIndex;
  std::atomic<int32_t> _running;
  std::vector<XtChannels> _channels;
  std::vector<XtIORingBuffers> _rings; 
  std::atomic<int32_t> _insideCallback;
  std::vector<XtAggregateContext> _contexts;
  std::vector<std::unique_ptr<XtBlockingStream>> _streams;

  XT_IMPLEMENT_STREAM();
  XtAggregateStream() = default;
  XtSystem GetSystem() const override;

  static void XT_CALLBACK
  OnSlaveBuffer(XtStream const* stream, XtBuffer const* buffer, void* user);
  static void XT_CALLBACK
  OnMasterBuffer(XtStream const* stream, XtBuffer const* buffer, void* user);
};

#endif // XT_PRIVATE_AGGREGATE_STREAM_HPP