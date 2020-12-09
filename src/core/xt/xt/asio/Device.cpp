#if XT_ENABLE_ASIO
#include <xt/asio/Shared.hpp>
#include <xt/asio/Private.hpp>

AsioDevice::
AsioDevice(CComPtr<IASIO> asio):
_asio(asio) { }
XtFault
AsioDevice::ShowControlPanel()
{ return _asio->controlPanel(); }
XtFault
AsioDevice::SupportsAccess(XtBool interleaved, XtBool* supports) const
{ *supports = !interleaved; return ASE_OK; }

XtFault
AsioDevice::GetChannelCount(XtBool output, int32_t* count) const
{
  long inputs, outputs;
  XT_VERIFY_ASIO(_asio->getChannels(&inputs, &outputs));
  *count = output? outputs: inputs;
  return ASE_OK;
}

XtFault
AsioDevice::GetChannelName(XtBool output, int32_t index, char* buffer, int32_t* size) const
{
  ASIOChannelInfo info = { 0 };
  XT_VERIFY_ASIO(XtiGetAsioChannelInfo(_asio, output, index, info));
  XtiCopyString(info.name, buffer, size);
  return ASE_OK;
}

XtFault
AsioDevice::GetBufferSize(XtFormat const* format, XtBufferSize* size) const
{  
  ASIOSampleRate rate;
  long min, max, preferred, granularity;
  XT_VERIFY_ASIO(_asio->getSampleRate(&rate));
  XT_VERIFY_ASIO(_asio->getBufferSize(&min, &max, &preferred, &granularity));
  size->min = min * 1000.0 / rate;
  size->max = max * 1000.0 / rate;
  size->current = preferred * 1000.0 / rate;
  return ASE_OK;
}

XtFault
AsioDevice::GetMix(XtBool* valid, XtMix* mix) const
{
  std::vector<ASIOChannelInfo> infos;
  XT_VERIFY_ASIO(XtiGetAsioChannelInfos(_asio, infos));
  if(infos.empty()) return ASE_OK;

  XtSample sample;
  ASIOSampleType type = infos[0].type;
  for(size_t i = 1; i < infos.size(); i++)
    if(infos[i].type != type) return XT_ASE_Format;
  if(!XtiSampleFromAsio(type, sample)) return XT_ASE_Format;

  ASIOSampleRate rate;
  ASIOError error = _asio->getSampleRate(&rate);
  if(error == ASE_NoClock || error == ASE_NotPresent) return ASE_OK;
  if(error != ASE_OK) return error;

  *valid = XtTrue;
  mix->sample = sample;
  mix->rate = static_cast<int32_t>(rate);
  return ASE_OK;
}

XtFault
AsioDevice::SupportsFormat(XtFormat const* format, XtBool* supports) const
{  
  ASIOSampleType type;
  if(!XtiSampleToAsio(format->mix.sample, type)) return ASE_OK;

  ASIOSampleRate rate;
  XT_VERIFY_ASIO(_asio->getSampleRate(&rate));
  if(static_cast<int32_t>(rate) != format->mix.rate) return ASE_OK;

  long inputs, outputs;
  XT_VERIFY_ASIO(_asio->getChannels(&inputs, &outputs));
  if(format->channels.inputs > inputs) return ASE_OK;
  if(format->channels.outputs > outputs) return ASE_OK;
  for(int32_t i = inputs; i < 64; i++)
    if((format->channels.inMask & (1ULL << i)) != 0) return ASE_OK;
  for(int32_t i = outputs; i < 64; i++)
    if((format->channels.outMask & (1ULL << i)) != 0) return ASE_OK;
  
  std::vector<ASIOChannelInfo> infos;  
  XT_VERIFY_ASIO(XtiGetAsioChannelInfos(_asio, infos));
  for(size_t i = 0; i < infos.size(); i++)
  {
    if(infos[i].type == type) continue;
    if(infos[i].isInput && XtiIsAsioChannelInUse(format->channels.inputs, 
      format->channels.inMask, infos[i].channel)) return ASE_OK;
    if(!infos[i].isInput && XtiIsAsioChannelInUse(format->channels.outputs,
      format->channels.outMask, infos[i].channel)) return ASE_OK;
  }

  *supports = XtTrue;
  return ASE_OK;
}

XtFault
AsioDevice::OpenStreamCore(XtDeviceStreamParams const* params, bool secondary, void* user, XtStream** stream)
{ 
  ASIOSampleRate rate;
  long min, max, preferred, granularity;
  XT_VERIFY_ASIO(_asio->getSampleRate(&rate));
  XT_VERIFY_ASIO(_asio->getBufferSize(&min, &max, &preferred, &granularity));

  long bufferSize;
  double wantedSize = params->bufferSize / 1000.0 * rate;
  if(wantedSize < min) bufferSize = min;
  else if(wantedSize > max) bufferSize = max;
  else
  {
    bufferSize = min;
    while(bufferSize < wantedSize) bufferSize += granularity == -1? bufferSize: granularity;
    if(bufferSize > max) bufferSize = max;
  }

  auto const& channels = params->format.channels;
  auto buffers = XtiAsioCreateBufferInfos(ASIOTrue, channels.inputs, channels.inMask);
  auto outputs = XtiAsioCreateBufferInfos(ASIOFalse, channels.outputs, channels.outMask);
  buffers.insert(buffers.end(), outputs.begin(), outputs.end());

  auto result = std::make_unique<AsioStream>();
  result->_runtime = std::make_unique<asmjit::JitRuntime>();
  auto bufferSwitch = XtiAsioJitBufferSwitch(result->_runtime.get(), &AsioStream::BufferSwitch, result.get());
  auto bufferSwitchTimeInfo = XtiAsioJitBufferSwitchTimeInfo(result->_runtime.get(), &AsioStream::BufferSwitchTimeInfo, result.get());
  result->_asio = _asio;
  result->_running.store(0);
  result->_buffers = buffers;
  result->_bufferSize = bufferSize;
  result->_insideCallback.store(0);
  result->_issueOutputReady = true;
  result->_callbacks.bufferSwitch = bufferSwitch;
  result->_callbacks.asioMessage = &XtiAsioMessage;
  result->_callbacks.bufferSwitchTimeInfo = bufferSwitchTimeInfo;
  result->_callbacks.sampleRateDidChange = &XtiAsioSampleRateDidChange;
  result->_inputs = std::vector<void*>(static_cast<size_t>(channels.inputs), nullptr);
  result->_outputs = std::vector<void*>(static_cast<size_t>(channels.outputs), nullptr);
  XT_VERIFY_ASIO(_asio->createBuffers(result->_buffers.data(), static_cast<long>(buffers.size()), bufferSize, &result->_callbacks));
  *stream = result.release();
  return ASE_OK;
}

#endif // XT_ENABLE_ASIO