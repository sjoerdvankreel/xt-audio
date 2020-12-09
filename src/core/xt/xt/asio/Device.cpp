#if XT_ENABLE_ASIO
#include <xt/asio/Shared.hpp>
#include <xt/asio/Private.hpp>

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
AsioDevice::GetChannelName(XtBool output, int32_t index, char* buffer, int32_t* size) const
{
  ASIOChannelInfo info = { 0 };
  XT_VERIFY_ASIO(XtiGetAsioChannelInfo(_asio, output, index, info));
  XtiCopyString(info.name, buffer, size);
  return ASE_OK;
}




XtFault AsioDevice::OpenStreamCore(const XtDeviceStreamParams* params, bool secondary, void* user, XtStream** stream) {
  
  double wantedSize;
  long asioBufferSize;
  ASIOSampleRate rate;
  std::vector<ASIOBufferInfo> buffers;
  long min, max, preferred, granularity;

  if(_streamOpen)
    return static_cast<XtFault>(DRVERR_DEVICE_ALREADY_OPEN);
  XT_VERIFY_ASIO(_asio->getSampleRate(&rate));
  XT_VERIFY_ASIO(_asio->getBufferSize(&min, &max, &preferred, &granularity));

  wantedSize = params->bufferSize / 1000.0 * rate;
  if(wantedSize < min)
    asioBufferSize = min;
  else if(wantedSize > max)
    asioBufferSize = max;
  else {
    asioBufferSize = min;
    while(asioBufferSize < wantedSize)
      asioBufferSize += granularity == -1? asioBufferSize: granularity;
    if(asioBufferSize > max)
      asioBufferSize = max;
  }

  CreateBufferInfos(buffers, ASIOTrue, params->format.channels.inputs, params->format.channels.inMask);
  CreateBufferInfos(buffers, ASIOFalse, params->format.channels.outputs, params->format.channels.outMask);
  auto result = std::make_unique<AsioStream>(this, params->format, asioBufferSize, buffers);
  result->callbacks.asioMessage = &AsioMessage;
  result->callbacks.sampleRateDidChange = &SampleRateDidChange;
  result->callbacks.bufferSwitch = JitBufferSwitch(result->runtime.get(), &BufferSwitch, result.get());
  result->callbacks.bufferSwitchTimeInfo = JitBufferSwitchTimeInfo(result->runtime.get(), &BufferSwitchTimeInfo, result.get());
  XT_VERIFY_ASIO(_asio->createBuffers(&result->buffers[0], static_cast<long>(buffers.size()), asioBufferSize, &result->callbacks));
  _streamOpen = true;
  *stream = result.release();
  return ASE_OK;
}

#endif // XT_ENABLE_ASIO