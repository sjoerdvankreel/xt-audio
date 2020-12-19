#include <xt/shared/Shared.hpp>
#include <xt/private/Stream.hpp>
#include <xt/aggregate/Stream.hpp>

XtSystem
XtAggregateStream::GetSystem() const
{ return _streams[_masterIndex]->GetSystem(); }
XtFault
XtAggregateStream::GetFrames(int32_t* frames) const
{ return *frames = _frames, 0; }
void
XtAggregateStream::StopMasterBuffer()
{ _streams[_masterIndex]->StopMasterBuffer(); }
XtFault
XtAggregateStream::StartMasterBuffer()
{ return _streams[_masterIndex]->StartMasterBuffer(); }
XtFault
XtAggregateStream::BlockMasterBuffer()
{ return _streams[_masterIndex]->BlockMasterBuffer(); }

void
XtAggregateStream::StopSlaveBuffer()
{
  _streams[_masterIndex]->StopSlaveBuffer();
  for(size_t i = 0; i < _streams.size(); i++)
    if(i != static_cast<size_t>(_masterIndex))
      _streams[i]->StopSlaveBuffer();
}

XtFault
XtAggregateStream::PrefillOutputBuffer()
{
  XtFault fault;
  for(size_t i = 0; i < _streams.size(); i++)
    if((fault = _streams[i]->PrefillOutputBuffer()) != 0) return fault;
  return 0;
}

XtFault
XtAggregateStream::StartSlaveBuffer()
{
  XtFault fault;
  for(size_t i = 0; i < _streams.size(); i++)
  {
    _rings[i].input.Clear();
    _rings[i].output.Clear();
  }

  auto guard = XtiGuard([this] { StopSlaveBuffer(); });
  for(size_t i = 0; i < _streams.size(); i++)
    if(i != static_cast<size_t>(_masterIndex))
      if((fault = _streams[i]->StartSlaveBuffer()) != 0) return fault;
  if((fault = _streams[_masterIndex]->StartSlaveBuffer()) != 0) return fault;
  guard.Commit();
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

XtFault 
XtAggregateStream::ProcessBuffer()
{
  auto& wi = _weave.input;
  auto& bi = _buffers.input;
  int32_t totalChannels = 0;
  bool interleaved = _params.interleaved;
  auto sampleSize = XtiGetSampleSize(_params.format.mix.sample);

  for(size_t i = 0; i < _streams.size(); i++)
    if(i != _masterIndex)
      _streams[i]->ProcessBuffer();
  _streams[_masterIndex]->ProcessBuffer();

  void* appInput = interleaved? static_cast<void*>(bi.interleaved.data()): bi.nonInterleaved.data();
  void* ringInput = interleaved? static_cast<void*>(wi.interleaved.data()): wi.nonInterleaved.data();
  for(size_t i = 0; i < _streams.size(); i++)
  {
    XtRingBuffer* ring = &_rings[i].input;
    XtBlockingStream const* str = _streams[i].get();
    XtFormat const* fmt = &_streams[i]->_params.format;
    int32_t thisIns = fmt->channels.inputs;
    if(thisIns > 0)
    {
      int32_t read = ring->Read(ringInput, buffer->frames);
      int32_t allIns = _params.format.channels.inputs;
      if(read < buffer->frames)
      {
        XtiZeroBuffer(ringInput, interleaved, read, thisIns, buffer->frames - read, sampleSize);
        OnXRun(i);
      }
      for(int32_t c = 0; c < thisIns; c++)
        XtiWeave(appInput, ringInput, interleaved, allIns, thisIns, totalChannels + c, c, buffer->frames, sampleSize);
      totalChannels += thisIns;
    }
  }

  auto& wo = _weave.output;
  auto& bo = _buffers.output; 
  void* appOutput = interleaved? static_cast<void*>(bo.interleaved.data()): bo.nonInterleaved.data();
  void* ringOutput = interleaved? static_cast<void*>(wo.interleaved.data()): wo.nonInterleaved.data();
  XtBuffer appBuffer = *buffer;
  appBuffer.input = appInput;
  appBuffer.output = appOutput;
  OnBuffer(-1, &appBuffer);

  totalChannels = 0;
  for(size_t i = 0; i < _streams.size(); i++)
  {
    XtRingBuffer* ring = &_rings[i].output;
    XtBlockingStream const* str = _streams[i].get();
    XtFormat const* fmt = &_streams[i]->_params.format;
    int32_t thisOuts = fmt->channels.outputs;
    if(thisOuts > 0)
    {
      int32_t allOuts = _params.format.channels.outputs;
      for(int32_t c = 0; c < thisOuts; c++)
        XtiWeave(ringOutput, appOutput, interleaved, thisOuts, allOuts, c, totalChannels + c, buffer->frames, sampleSize);
      totalChannels += thisOuts;
      int32_t written = ring->Write(ringOutput, buffer->frames);
      if(written < buffer->frames) OnXRun(i);
    }
  }
  return 0;
}