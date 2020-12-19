#include <xt/aggregate/Runner.hpp>

XtAggregateRunner::
XtAggregateRunner(XtAggregateStream* stream):
_stream(stream), XtBlockingRunner(stream) { }

uint32_t
XtAggregateRunner::OnBuffer(int32_t index, XtBuffer const* buffer)
{
  XT_ASSERT(index != -1);
  XtBool interleaved = _stream->_params.interleaved;
  XtChannels const* channels = &_stream->_channels[index];
  auto sampleSize = XtiGetSampleSize(_stream->_params.format.mix.sample);

  if(buffer->input != nullptr)
  { 
    XtRingBuffer* inputRing = &_stream->_rings[index].input;
    int32_t written = inputRing->Write(buffer->input, buffer->frames);
    if(written < buffer->frames) OnXRun(index);
  }
  if(buffer->output != nullptr)
  { 
    XtRingBuffer* outputRing = &_stream->_rings[index].output;
    int32_t read = outputRing->Read(buffer->output, buffer->frames);
    if(read < buffer->frames)
    {
      XtiZeroBuffer(buffer->output, interleaved, read, channels->outputs, buffer->frames - read, sampleSize);
      OnXRun(index);
    }
  }
  return 0;
}