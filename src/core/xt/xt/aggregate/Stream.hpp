#ifndef XT_AGGREGATE_STREAM_HPP
#define XT_AGGREGATE_STREAM_HPP

#include <xt/blocking/Stream.hpp>
#include <xt/aggregate/RingBuffer.hpp>

#include <vector>
#include <memory>

struct XtAggregateStream final: 
public XtBlockingStream
{
  int32_t _frames;
  XtIOBuffers _weave;  
  int32_t _masterIndex;
  std::vector<XtBool> _emulated;
  std::vector<XtIOBuffers> _buffers;
  std::vector<XtChannels> _channels;
  std::vector<XtIORingBuffers> _rings;
  std::vector<std::unique_ptr<XtBlockingStream>> _streams;

  ~XtAggregateStream() { };
  XtAggregateStream() = default;
  XtSystem GetSystem() const override;

  XT_IMPLEMENT_STREAM_BASE();
  XT_IMPLEMENT_BLOCKING_STREAM();
};

#endif // XT_AGGREGATE_STREAM_HPP