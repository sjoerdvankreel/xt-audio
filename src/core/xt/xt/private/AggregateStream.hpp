#ifndef XT_PRIVATE_AGGREGATE_STREAM_HPP
#define XT_PRIVATE_AGGREGATE_STREAM_HPP

#include <xt/private/BlockingStream.hpp>
#include <xt/private/RingBuffer.hpp>
#include <vector>
#include <memory>

struct XtAggregateStream: 
public XtBlockingStream
{
  int32_t _frames;
  XtIOBuffers _weave;
  int32_t _masterIndex;
  std::vector<XtChannels> _channels;
  std::vector<XtIORingBuffers> _rings;
  std::vector<std::unique_ptr<XtBlockingStream>> _streams;

  XT_IMPLEMENT_STREAM_BASE();
  XT_IMPLEMENT_BLOCKING_STREAM();
  
  XtAggregateStream() = default;
  XtSystem GetSystem() const override;

  static uint32_t XT_CALLBACK
  OnBuffer(XtStream const* stream, XtBuffer const* buffer, void* user);
};

#endif // XT_PRIVATE_AGGREGATE_STREAM_HPP