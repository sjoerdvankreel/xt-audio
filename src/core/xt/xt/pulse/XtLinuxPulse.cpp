#if XT_ENABLE_PULSE
#include <xt/api/private/Platform.hpp>
#include <xt/api/private/Service.hpp>
#include <xt/api/private/Device.hpp>
#include <xt/api/private/Stream.hpp>
#include <xt/private/BlockingStream.hpp>
#include <xt/private/Shared.hpp>
#include <xt/private/Services.hpp>
#include <xt/pulse/Shared.hpp>
#include <pulse/simple.h>
#include <pulse/pulseaudio.h>
#include <memory>
#include <vector>

// ---- local ----



// ---- forward ----



// ---- local ----

// ---- service ----

XtCapabilities PulseService::GetCapabilities() const {
  return static_cast<XtCapabilities>(XtCapabilitiesAggregation | XtCapabilitiesChannelMask);
}

XtFault PulseService::GetDeviceCount(int32_t* count) const {
  XtFault fault;
  XtPaSimple pa;
  if((fault = XtiCreatePulseDefaultClient(XtTrue, &pa.pa)) != PA_OK) return fault;
  *count = pa.pa == nullptr? 0: 2;
  return PA_OK;
}

XtFault PulseService::OpenDevice(int32_t index, XtDevice** device) const {
  return OpenDefaultDevice(index != 0, device);
}

XtFault PulseService::OpenDefaultDevice(XtBool output, XtDevice** device) const {
  XtFault fault;
  XtPaSimple pa;
  if((fault = XtiCreatePulseDefaultClient(output, &pa.pa)) != PA_OK) return fault;
  *device = new PulseDevice(output);
  return PA_OK;
}

// ---- device ----

XtFault PulseDevice::ShowControlPanel() {
  return 0;
}

XtFault PulseDevice::GetName(char* buffer, int32_t* size) const {
  XtiCopyString(_output? "Output": "Input", buffer, size);
  return PA_OK;
}

XtFault PulseDevice::SupportsAccess(XtBool interleaved, XtBool* supports) const {
  *supports = interleaved;
  return PA_OK;
}

XtFault PulseDevice::GetMix(XtBool* valid, XtMix* mix) const {
  *valid = XtTrue;
  mix->rate = XtiPaDefaultRate;
  mix->sample = XtiPaDefaultSample;
  return PA_OK;
}

XtFault PulseDevice::GetChannelCount(XtBool output, int32_t* count) const {
  *count = _output != output? 0: PA_CHANNEL_POSITION_MAX;
  return PA_OK;
}

XtFault PulseDevice::GetBufferSize(const XtFormat* format, XtBufferSize* size) const {  
  size->min = XtiPaMinBufferSize;
  size->max = XtiPaMaxBufferSize;
  size->current = XtiPaDefaultBufferSize;
  return PA_OK;
}

XtFault PulseDevice::GetChannelName(XtBool output, int32_t index, char* buffer, int32_t* size) const {
  XtiCopyString(pa_channel_position_to_pretty_string(static_cast<pa_channel_position_t>(index)), buffer, size);
  return PA_OK;
}

XtFault PulseDevice::SupportsFormat(const XtFormat* format, XtBool* supports) const {
  pa_sample_format pulse;
  if(format->channels.inputs > 0 && _output)
    return PA_OK;
  if(format->channels.outputs > 0 && !_output)
    return PA_OK;
  if(format->mix.rate < XtiPaMinRate)
    return PA_OK;
  if(format->mix.rate > XtiPaMaxRate)
    return PA_OK;
  if(format->channels.inputs >= PA_CHANNEL_POSITION_MAX)
    return PA_OK;
  if(format->channels.outputs >= PA_CHANNEL_POSITION_MAX)
    return PA_OK;
  for(int32_t i = PA_CHANNEL_POSITION_MAX; i < 64; i++)
    if(format->channels.inMask & (1ULL << i) || format->channels.outMask & (1ULL << i))
      return PA_OK;
  *supports = XtTrue;
  return PA_OK;
}

XtFault PulseDevice::OpenStreamCore(const XtDeviceStreamParams* params, bool secondary, void* user, XtStream** stream) {

  uint64_t mask;
  pa_simple* client;
  int fault = PA_OK;
  int32_t frameSize;
  pa_channel_map map;
  pa_sample_spec spec;
  int32_t bufferFrames;
  std::unique_ptr<PulseStream> result;

  bufferFrames = static_cast<int32_t>(params->bufferSize / 1000.0 * params->format.mix.rate);
  spec.rate = params->format.mix.rate;
  spec.format = XtiSampleToPulse(params->format.mix.sample);
  spec.channels = params->format.channels.inputs + params->format.channels.outputs;
  mask = params->format.channels.inMask? params->format.channels.inMask: params->format.channels.outMask;
  if(mask == 0)
    pa_channel_map_init_extend(&map, spec.channels, PA_CHANNEL_MAP_DEFAULT);
  else {
    pa_channel_map_init(&map);
    map.channels = spec.channels;
    for(int32_t i = 0, j = 0; i < 64; i++)
      if(mask & (1ULL << i))
        map.map[j++] = static_cast<pa_channel_position_t>(i);
  }
  
  frameSize = (params->format.channels.inputs + params->format.channels.outputs) * XtiGetSampleSize(params->format.mix.sample);
  auto id = XtPlatform::instance->_id.c_str();
  auto dir = _output? PA_STREAM_PLAYBACK: PA_STREAM_RECORD;
  if((client = pa_simple_new(nullptr, id, dir, nullptr, 
    id, &spec, &map, nullptr, &fault)) == nullptr)
    return fault;
  *stream = new PulseStream(secondary, XtPaSimple(client), _output, bufferFrames, frameSize);
  return PA_OK;
}

// ---- stream ----

void PulseStream::StopStream() {
}

void PulseStream::StartStream() {
}

XtFault PulseStream::GetFrames(int32_t* frames) const {
  *frames = _bufferFrames;
  return PA_OK;
}

XtFault PulseStream::GetLatency(XtLatency* latency) const {
  return PA_OK;
}

void PulseStream::ProcessBuffer(bool prefill) {
  int fault;
  XtBuffer xtBuffer = { 0 };
  void* inData = _output? nullptr: &_audio[0];
  void* outData = !_output? nullptr: &_audio[0];

  if(!_output && pa_simple_read(_client.pa, &_audio[0], _audio.size(), &fault) < 0) {
    XT_VERIFY_ON_BUFFER(fault);
    return;
  }
  xtBuffer.input = inData;
  xtBuffer.output = outData;
  xtBuffer.frames = _bufferFrames;
  OnBuffer(&xtBuffer);
  if(_output && pa_simple_write(_client.pa, &_audio[0], _audio.size(), &fault) < 0)
    XT_VERIFY_ON_BUFFER(fault);
}

#endif // XT_ENABLE_PULSE