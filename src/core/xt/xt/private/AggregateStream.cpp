#include <xt/private/AggregateStream.hpp>
#include <xt/private/Shared.hpp>

XtSystem
XtAggregateStream::GetSystem() const
{ return _system; }

XtFault
XtAggregateStream::GetFrames(int32_t* frames) const
{ return *frames = _frames, 0; }

XtFault
XtAggregateStream::Stop()
{
  XtFault fault = 0;
  XtFault result = 0;
  if(!XtiCompareExchange(_running, 1, 0)) return 0;
  while(_insideCallbackCount.load() != 0);
  if((fault = _streams[_masterIndex]->Stop()) != 0) result = fault;
  for(size_t i = 0; i < _streams.size(); i++)
    if(i != static_cast<size_t>(_masterIndex))
      if((fault = _streams[i]->Stop()) != 0) result = fault;
  return result;
}

XtFault
XtAggregateStream::Start()
{
  XtFault fault = 0;
  XtFault result = 0;
  for(size_t i = 0; i < _streams.size(); i++)
  {
    _rings[i].input.Clear();
    _rings[i].output.Clear();
  }
  for(size_t i = 0; i < _streams.size(); i++)
    if(i != static_cast<size_t>(_masterIndex))
      if((fault = _streams[i]->Start()) != 0) result = fault;
  if((fault = _streams[_masterIndex]->Start()) != 0) result = fault;
  if(fault != 0)
  {
    for(size_t i = 0; i < _streams.size(); i++)
      _streams[i]->Stop();
    return fault;
  }
  XT_ASSERT(XtiCompareExchange(_running, 0, 1));
  return 0;
}

XtFault
XtAggregateStream::GetLatency(XtLatency* latency) const 
{
  XtFault fault;
  XtLatency local = { 0 };
  auto invRate = 1000.0 / _params.format.mix.rate;
  for(size_t i = 0; i < _streams.size(); i++)
  {
    if((fault = _streams[i]->GetLatency(&local)) != 0) return fault;
    if(local.input == 0.0 && local.output == 0.0) return 0;
    if(local.input > 0.0)
    {
      local.input += _rings[i].input.Full() * invRate;
      latency->input = local.input > latency->input? local.input: latency->input;
    }      
    if(local.output > 0.0)
    {
      local.output += _rings[i].output.Full() * invRate;
      latency->output = local.output > latency->output? local.output: latency->output;
    }      
  }
  return 0;
}

void XT_CALLBACK 
XtAggregateStream::OnSlaveBuffer(XtStream const* stream, XtBuffer const* buffer, void* user)
{
  auto ctx = static_cast<XtAggregateContext*>(user);
  int32_t index = ctx->index;
  XtAggregateStream* aggregate = ctx->stream;
  XtOnXRun onXRun = aggregate->_params.stream.onXRun;
  XtChannels const* channels = &aggregate->_channels[index];
  XtBool interleaved = aggregate->_params.stream.interleaved;
  auto sampleSize = XtiGetSampleSize(aggregate->_params.format.mix.sample);

  aggregate->_insideCallbackCount++;
  if(buffer->error != 0) 
  {
    for(int32_t i = 0; i < aggregate->_streams.size(); i++)
      if(i != static_cast<size_t>(index))
        aggregate->_streams[i]->RequestStop();
    aggregate->_params.stream.onBuffer(aggregate, buffer, aggregate->_user);
    aggregate->_insideCallbackCount--;
    return;
  }
  if(aggregate->_running.load() != 1)
  {
    XtiZeroBuffer(buffer->output, interleaved, 0, channels->outputs, buffer->frames, sampleSize);
    aggregate->_insideCallbackCount--;
    return;
  }
  if(buffer->input != nullptr)
  { 
    XtRingBuffer* inputRing = &aggregate->_rings[index].input;
    int32_t written = inputRing->Write(buffer->input, buffer->frames);
    if(written < buffer->frames && onXRun != nullptr)
      onXRun(-1, aggregate->_user);
  }
  if(buffer->output != nullptr)
  { 
    XtRingBuffer* outputRing = &aggregate->_rings[index].output;
    int32_t read = outputRing->Read(buffer->output, buffer->frames);
    if(read < buffer->frames)
    {
      XtiZeroBuffer(buffer->output, interleaved, read, channels->outputs, buffer->frames - read, sampleSize);
      if(onXRun != nullptr)
        onXRun(-1, aggregate->_user);
    }
  }
  aggregate->_insideCallbackCount--;
}

void XT_CALLBACK 
XtAggregateStream::OnMasterBuffer(XtStream const* stream, XtBuffer const* buffer, void* user)
{
  auto ctx = static_cast<XtAggregateContext*>(user);
  int32_t index = ctx->index;
  XtAggregateStream* aggregate = ctx->stream;
  XtFormat const* format = &aggregate->_params.format;
  XtOnXRun onXRun = aggregate->_params.stream.onXRun;
  XtBool interleaved = aggregate->_params.stream.interleaved;
  XtChannels const* channels = &aggregate->_channels[index];
  XtRingBuffer* inputRing = &aggregate->_rings[index].input;
  XtRingBuffer* outputRing = &aggregate->_rings[index].output;
  int32_t sampleSize = XtiGetSampleSize(aggregate->_params.format.mix.sample);

  for(size_t i = 0; i < aggregate->_streams.size(); i++)
    if(i != aggregate->_masterIndex)
      aggregate->_streams[i]->ProcessBuffer(false);

  OnSlaveBuffer(stream, buffer, user);
  if(buffer->error != 0) return;
  if(aggregate->_running.load() != 1) return;
  aggregate->_insideCallbackCount++;

  int32_t totalChannels = 0;
  auto& wi = aggregate->_weave.input;
  auto& bi = aggregate->_buffers.input;
  void* appInput = interleaved? static_cast<void*>(bi.interleaved.data()): bi.nonInterleaved.data();
  void* ringInput = interleaved? static_cast<void*>(wi.interleaved.data()): wi.nonInterleaved.data();
  for(size_t i = 0; i < aggregate->_streams.size(); i++)
  {
    XtRingBuffer* ring = &aggregate->_rings[i].input;
    XtStream const* str = aggregate->_streams[i].get();
    XtFormat const* fmt = &aggregate->_streams[i]->_params.format;
    if(fmt->channels.inputs > 0)
    {
      int32_t read = ring->Read(ringInput, buffer->frames);
      if(read < buffer->frames)
      {
        XtiZeroBuffer(ringInput, interleaved, read, fmt->channels.inputs, buffer->frames - read, sampleSize);
        if(onXRun != nullptr) onXRun(-1, aggregate->_user);
      }
      for(int32_t c = 0; c < fmt->channels.inputs; c++)
        XtiWeave(appInput, ringInput, interleaved, format->channels.inputs, fmt->channels.inputs, totalChannels + c, c, buffer->frames, sampleSize);
      totalChannels += fmt->channels.inputs;
    }
  }

  auto& wo = aggregate->_weave.output;
  auto& bo = aggregate->_buffers.output; 
  void* appOutput = interleaved? static_cast<void*>(bo.interleaved.data()): bo.nonInterleaved.data();
  void* ringOutput = interleaved? static_cast<void*>(wo.interleaved.data()): wo.nonInterleaved.data();
  XtBuffer appBuffer = *buffer;
  appBuffer.input = appInput;
  appBuffer.output = appOutput;
  aggregate->_params.stream.onBuffer(aggregate, &appBuffer, aggregate->_user);

  totalChannels = 0;
  for(size_t i = 0; i < aggregate->_streams.size(); i++)
  {
    XtRingBuffer* ring = &aggregate->_rings[i].output;
    XtStream const* str = aggregate->_streams[i].get();
    XtFormat const* fmt = &aggregate->_streams[i]->_params.format;
    if(fmt->channels.outputs > 0)
    {
      for(int32_t c = 0; c < fmt->channels.outputs; c++)
        XtiWeave(ringOutput, appOutput, interleaved, fmt->channels.outputs, format->channels.outputs, c, totalChannels + c, buffer->frames, sampleSize);
      totalChannels += fmt->channels.outputs;
      int32_t written = ring->Write(ringOutput, buffer->frames);
      if(written < buffer->frames && onXRun != nullptr)
        onXRun(-1, aggregate->_user);
    }
  }

  aggregate->_insideCallbackCount--;
}