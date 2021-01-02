#include <xt/aggregate/Runner.hpp>

XtAggregateRunner::
XtAggregateRunner(XtAggregateStream* stream):
_stream(stream), XtBlockingRunner(stream) { }

XtFault
XtAggregateRunner::OnBuffer(int32_t index, XtBuffer const* buffer)
{
  XtOnBufferParams params = { 0 };
  params.index = index;
  params.buffer = buffer;
  params.buffers = &_stream->_buffers[index];
  params.emulated = _stream->_emulated[index];
  params.interleaved = _params.stream.interleaved;
  params.format = &_stream->_streams[index]->_params.format;
  if(index == _stream->_masterIndex)
    return XtiOnBuffer(&params, [this, index](XtBuffer const* converted) {
      return OnMasterBuffer(index, converted); });
  return XtiOnBuffer(&params, [this, index](XtBuffer const* converted) { 
    return OnSlaveBuffer(index, converted); });
}

XtFault
XtAggregateRunner::OnSlaveBuffer(int32_t index, XtBuffer const* buffer)
{
  XtBool interleaved = _params.stream.interleaved;
  XtChannels const* channels = &_stream->_channels[index];
  auto sampleSize = XtiGetSampleSize(_params.format.mix.sample);

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

XtFault
XtAggregateRunner::OnMasterBuffer(int32_t index, XtBuffer const* buffer)
{
  XtFault fault;
  XtBool interleaved = _params.stream.interleaved;
  XtChannels const* channels = &_stream->_channels[index];
  XtRingBuffer* inputRing = &_stream->_rings[index].input;
  XtRingBuffer* outputRing = &_stream->_rings[index].output;
  int32_t sampleSize = XtiGetSampleSize(_params.format.mix.sample);

  for(size_t i = 0; i < _stream->_streams.size(); i++)
    if(i != _stream->_masterIndex)
      if((fault = _stream->_streams[i]->ProcessBuffer()) != 0) return fault;
  if((fault = OnSlaveBuffer(index, buffer)) != 0) return fault;

  int32_t totalChannels = 0;
  auto& wi = _stream->_weave.input;
  auto& bi = _buffers.input;
  void* appInput = interleaved? static_cast<void*>(bi.interleaved.data()): bi.nonInterleaved.data();
  void* ringInput = interleaved? static_cast<void*>(wi.interleaved.data()): wi.nonInterleaved.data();
  for(size_t i = 0; i < _stream->_streams.size(); i++)
  {
    XtRingBuffer* ring = &_stream->_rings[i].input;
    XtBlockingStream const* str = _stream->_streams[i].get();
    XtFormat const* fmt = &_stream->_streams[i]->_params.format;
    int32_t thisIns = fmt->channels.inputs;
    if(thisIns > 0)
    {
      int32_t read = ring->Read(ringInput, buffer->frames);
      int32_t allIns = _stream->_params.format.channels.inputs;
      if(read < buffer->frames)
      {
        XtiZeroBuffer(ringInput, interleaved, read, thisIns, buffer->frames - read, sampleSize);
        OnXRun(index);
      }
      for(int32_t c = 0; c < thisIns; c++)
        XtiWeave(appInput, ringInput, interleaved, allIns, thisIns, totalChannels + c, c, buffer->frames, sampleSize);
      totalChannels += thisIns;
    }
  }

  auto& wo = _stream->_weave.output;
  auto& bo = _buffers.output; 
  void* appOutput = interleaved? static_cast<void*>(bo.interleaved.data()): bo.nonInterleaved.data();
  void* ringOutput = interleaved? static_cast<void*>(wo.interleaved.data()): wo.nonInterleaved.data();
  XtBuffer appBuffer = *buffer;
  appBuffer.input = appInput;
  appBuffer.output = appOutput;
  if((fault = _params.stream.onBuffer(this, &appBuffer, _user)) != 0) return fault;

  totalChannels = 0;
  for(size_t i = 0; i < _stream->_streams.size(); i++)
  {
    XtRingBuffer* ring = &_stream->_rings[i].output;
    XtBlockingStream const* str = _stream->_streams[i].get();
    XtFormat const* fmt = &_stream->_streams[i]->_params.format;
    int32_t thisOuts = fmt->channels.outputs;
    if(thisOuts > 0)
    {
      int32_t allOuts = _stream->_params.format.channels.outputs;
      for(int32_t c = 0; c < thisOuts; c++)
        XtiWeave(ringOutput, appOutput, interleaved, thisOuts, allOuts, c, totalChannels + c, buffer->frames, sampleSize);
      totalChannels += thisOuts;
      int32_t written = ring->Write(ringOutput, buffer->frames);
      if(written < buffer->frames) OnXRun(index);
    }
  }
  return 0;
}
