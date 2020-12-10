#if XT_ENABLE_JACK
#include <xt/jack/Shared.hpp>
#include <xt/jack/Private.hpp>
#include <xt/api/private/Platform.hpp>
#include <memory>
#include <utility>

JackDevice::
JackDevice(XtJackClient&& jc):
_jc(std::move(jc)) { }
XtFault
JackDevice::ShowControlPanel()
{ return 0; }
XtFault 
JackDevice::GetChannelCount(XtBool output, int32_t* count) const
{ *count = XtiJackCountPorts(_jc.jc, output); return 0; }
XtFault
JackDevice::SupportsAccess(XtBool interleaved, XtBool* supports) const
{ *supports = !interleaved; return 0; }

XtFault
JackDevice::GetMix(XtBool* valid, XtMix* mix) const
{
  *valid = XtTrue;
  mix->sample = XtSampleFloat32;
  mix->rate = jack_get_sample_rate(_jc.jc);
  return 0;
}

XtFault 
JackDevice::SupportsFormat(const XtFormat* format, XtBool* supports) const
{
  auto const& channels = format->channels;
  if(format->mix.sample != XtSampleFloat32) return 0;
  if(format->mix.rate != jack_get_sample_rate(_jc.jc)) return 0;
  if(channels.inputs > XtiJackCountPorts(_jc.jc, XtFalse)) return 0;
  if(channels.outputs > XtiJackCountPorts(_jc.jc, XtTrue)) return 0;
  for(int32_t i = XtiJackCountPorts(_jc.jc, XtFalse); i < 64; i++)
    if((format->channels.inMask & (1ULL << i)) != 0) return 0;
  for(int32_t i = XtiJackCountPorts(_jc.jc, XtTrue); i < 64; i++)
    if((format->channels.outMask & (1ULL << i)) != 0) return 0;
  *supports = XtTrue;
  return 0;
}

XtFault
JackDevice::GetBufferSize(XtFormat const* format, XtBufferSize* size) const
{
  jack_nframes_t rate = jack_get_sample_rate(_jc.jc);
  size->current = jack_get_buffer_size(_jc.jc) * 1000.0 / rate;
  size->min = size->current;
  size->max = size->current;
  return 0;
}

XtFault
JackDevice::GetChannelName(XtBool output, int32_t index, char* buffer, int32_t* size) const
{
  unsigned long flag = output? JackPortIsInput: JackPortIsOutput;
  XtJackPtr<const char*> ports(jack_get_ports(_jc.jc, nullptr, JACK_DEFAULT_AUDIO_TYPE, flag));
  if(index >= XtiJackCountPorts(_jc.jc, output)) return ENODEV;
  XtiCopyString(ports.p[index], buffer, size);
  return 0;
}

XtFault 
JackDevice::OpenStreamCore(XtDeviceStreamParams const* params, bool secondary, void* user, XtStream** stream)
{  
  XtFault fault;
  auto id = XtPlatform::instance->_id.c_str();
  jack_client_t* client = jack_client_open(id, JackNullOption, nullptr);
  if(client == nullptr) return ESRCH;
  XtJackClient jc(client);
  
  std::vector<XtJackPort> inputs;
  std::vector<XtJackPort> outputs;
  auto const& channels = params->format.channels;
  if((fault = XtiJackCreatePorts(client, channels.inputs, channels.inMask, JackPortIsInput, inputs)) != 0) return fault;
  if((fault = XtiJackCreatePorts(client, channels.outputs, channels.outMask, JackPortIsOutput, outputs)) != 0) return fault;

  size_t sampleSize = XtiGetSampleSize(params->format.mix.sample);
  size_t bufferFrames = jack_get_buffer_size(client);
  auto result = std::make_unique<JackStream>(std::move(jc));
  result->_inputs = std::move(inputs);
  result->_outputs = std::move(outputs);
  result->_inputChannels = std::vector<void*>(static_cast<size_t>(channels.inputs), nullptr);
  result->_outputChannels = std::vector<void*>(static_cast<size_t>(channels.outputs), nullptr);
  if((fault = jack_set_xrun_callback(client, &JackStream::XRunCallback, result.get())) != 0) return fault;
  if((fault = jack_set_process_callback(client, &JackStream::ProcessCallback, result.get())) != 0) return fault;
  *stream = result.release();
  return 0;
}

#endif // XT_ENABLE_JACK