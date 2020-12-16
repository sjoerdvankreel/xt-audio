#if XT_ENABLE_ASIO
#include <xt/asio/Shared.hpp>
#include <xt/asio/Private.hpp>

XtFault
AsioStream::GetFrames(int32_t* frames) const
{ *frames = _bufferSize; return ASE_OK; }
AsioStream::
~AsioStream()
{ XT_ASSERT(XtiIsAsioSuccess(_asio->disposeBuffers())); }

XtBool
AsioStream::IsRunning() const
{ return _running.load() != 0; }

XtFault
AsioStream::Start()
{
  XT_ASSERT(XtiCompareExchange(_running, 0, 1));
  XtFault fault = _asio->start();
  if(fault != ASE_OK)
  {
    XT_ASSERT(XtiCompareExchange(_running, 1, 0));
    return fault;
  }
  OnRunning(XtTrue);
  return ASE_OK;
}

XtFault
AsioStream::Stop()
{
  if(!XtiCompareExchange(_running, 1, 0)) return ASE_OK;
  while(_insideCallback.load() == 1);
  XtFault fault = _asio->stop();
  OnRunning(XtFalse);
  return fault;
}

XtFault
AsioStream::GetLatency(XtLatency* latency) const
{
  long input, output;
  ASIOSampleRate rate;
  auto const& channels = _params.format.channels;
  XT_VERIFY_ASIO(_asio->getSampleRate(&rate));
  XT_VERIFY_ASIO(_asio->getLatencies(&input, &output));
  latency->input = channels.inputs == 0? 0.0: input * 1000.0 / rate;
  latency->output = channels.outputs == 0? 0.0: output * 1000.0 / rate;
  return ASE_OK;
}

void XT_ASIO_CALL
AsioStream::BufferSwitch(long index, ASIOBool direct, void* ctx)
{
  ASIOTime time = { 0 };
  AsioTimeInfo& info = time.timeInfo;
  AsioStream* stream = static_cast<AsioStream*>(ctx);
	if(stream->_asio->getSamplePosition(&info.samplePosition, &info.systemTime) == ASE_OK)
		info.flags = kSystemTimeValid | kSamplePositionValid;
	BufferSwitchTimeInfo(&time, index, direct, ctx);
}

ASIOTime* XT_ASIO_CALL
AsioStream::BufferSwitchTimeInfo(ASIOTime* time, long index, ASIOBool direct, void* ctx)
{
  AsioStream* s = static_cast<AsioStream*>(ctx);
  if(!direct) return nullptr;
  if(s->_running.load() != 1) return nullptr;
  if(!XtiCompareExchange(s->_insideCallback, 0, 1)) return nullptr;

  XtBuffer buffer = { 0 };
  buffer.frames = s->_bufferSize;
  auto const& channels = s->_params.format.channels;
  buffer.input = channels.inputs > 0? s->_inputs.data(): nullptr;
  buffer.output = channels.outputs > 0? s->_outputs.data(): nullptr;
  for(size_t i = 0; i < channels.inputs; i++)
    s->_inputs[i] = s->_buffers[i].buffers[index];
  for(size_t i = 0; i < channels.outputs; i++)
    s->_outputs[i] = s->_buffers[channels.inputs + i].buffers[index];

  double const nsPerMs = 1000000.0;
  AsioTimeInfo& info = time->timeInfo;
  if(info.flags & kSamplePositionValid && info.flags & kSystemTimeValid)
  {
    buffer.timeValid = XtTrue;
    buffer.time = XT_ASIO_TO_UINT64(info.systemTime.lo, info.systemTime.hi) / nsPerMs;
    buffer.position = XT_ASIO_TO_UINT64(info.samplePosition.lo, info.samplePosition.hi);
  }

  s->OnBuffer(&buffer);
  if(s->_issueOutputReady)
    s->_issueOutputReady = s->_asio->outputReady() == ASE_OK;
  XT_ASSERT(XtiCompareExchange(s->_insideCallback, 1, 0));
  return nullptr; 
}

#endif // XT_ENABLE_ASIO