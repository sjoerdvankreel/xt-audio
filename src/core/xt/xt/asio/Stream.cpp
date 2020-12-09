#if XT_ENABLE_ASIO
#include <xt/asio/Shared.hpp>
#include <xt/asio/Private.hpp>

XtFault
AsioStream::GetFrames(int32_t* frames) const
{ *frames = _bufferSize; return ASE_OK; }
AsioStream::
~AsioStream()
{ XT_ASSERT(XtiIsAsioSuccess(_asio->disposeBuffers())); }

XtFault
AsioStream::Start()
{
  XT_ASSERT(XtiCompareExchange(_running, 0, 1));
  return _asio->start();
}

XtFault
AsioStream::Stop()
{
  if(!XtiCompareExchange(_running, 1, 0)) return ASE_OK;
  while(_insideCallback.load() == 1);
  return _asio->stop();
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

#endif // XT_ENABLE_ASIO