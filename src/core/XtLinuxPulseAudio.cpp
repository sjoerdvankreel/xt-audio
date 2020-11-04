#ifdef __linux__
#include "XtLinux.hpp"

#ifdef XT_DISABLE_PULSE_AUDIO
const XtService* XtiServicePulseAudio = nullptr;
#else // XT_DISABLE_PULSE_AUDIO

#include <memory>
#include <vector>
#include <pulse/simple.h>
#include <pulse/pulseaudio.h>

// ---- local ----

static const int32_t XtPaMinRate = 1;
static const int32_t XtPaMaxRate = 192000;
static const int32_t XtPaDefaultRate = 44100;
static const int32_t XtPaDefaultChannels = 2;
static const double XtPaMinBufferSize = 1.0;
static const double XtPaMaxBufferSize = 2000.0;
static const double XtPaDefaultBufferSize = 80.0;
static const XtFault XtPaErrFormat = PA_ERR_MAX + 1;
static const XtSample XtPaDefaultSample = XtSampleInt16;

struct XtPaSimple {
  pa_simple* simple;
  XtPaSimple(const XtPaSimple&) = delete;
  XtPaSimple& operator=(const XtPaSimple&) = delete;

  XtPaSimple(XtPaSimple&& rhs):
  simple(rhs.simple)
  { rhs.simple = nullptr; }

  XtPaSimple(pa_simple* simple):
  simple(simple) {}

  XtPaSimple& operator=(XtPaSimple&& rhs) {
    simple = rhs.simple;
    rhs.simple = nullptr;
    return *this;
  }

  ~XtPaSimple() 
  { if(simple != nullptr) pa_simple_free(simple); }
};

// ---- forward ----

XT_DECLARE_SERVICE(PulseAudio, PulseAudio);

struct PulseAudioDevice: public XtDevice {
  const bool output;
  XT_IMPLEMENT_DEVICE(PulseAudio);
  PulseAudio(bool output):
  output(output) {}
};

struct PulseAudioStream: public XtlLinuxBlockingStream {
  const bool output;
  const XtPaSimple client;
  std::vector<char> audio;
  const int32_t bufferFrames;
  XT_IMPLEMENT_BLOCKING_STREAM(PulseAudio);

  ~PulseAudioStream() { Stop(); }
  PulseAudioStream(bool secondary, XtPaSimple&& c, bool output, int32_t bufferFrames, int32_t frameSize):
  XtlLinuxBlockingStream(secondary), output(output), client(std::move(c)), 
  audio(static_cast<size_t>(bufferFrames * frameSize), '\0'),
  bufferFrames(bufferFrames) 
  { XT_ASSERT(client.simple != nullptr); }
};

// ---- local ----

static pa_sample_format ToPulseAudioSample(XtSample sample) {
  switch(sample) {
  case XtSampleUInt8: return PA_SAMPLE_U8; 
  case XtSampleInt16: return PA_SAMPLE_S16LE; 
  case XtSampleInt24: return PA_SAMPLE_S24LE; 
  case XtSampleInt32: return PA_SAMPLE_S32LE; 
  case XtSampleFloat32: return PA_SAMPLE_FLOAT32LE;
  default: return XT_FAIL("Unknown sample."), PA_SAMPLE_U8;
  }
}

static XtPaSimple CreateDefaultClient(XtBool output) {
  pa_sample_spec spec;
  spec.rate = XtPaDefaultRate;
  spec.channels = XtPaDefaultChannels;
  spec.format = ToPulseAudioSample(XtPaDefaultSample);
  auto dir = output? PA_STREAM_PLAYBACK: PA_STREAM_RECORD;
  return XtPaSimple(pa_simple_new(nullptr, XtiId, dir,
    nullptr, XtiId, &spec, nullptr, nullptr, nullptr));
}

// ---- service ----

XtFault PulseAudioService::GetFormatFault() const {
  return XtPaErrFormat;
}

XtCapabilities PulseAudioService::GetCapabilities() const {
  return XtCapabilitiesChannelMask;
}

const char* PulseAudioService::GetFaultText(XtFault fault) const {
  return fault == XtPaErrFormat? "XtPaErrFormat": pa_strerror(fault);
}

XtFault PulseAudioService::GetDeviceCount(int32_t* count) const {
  XtPaSimple client = CreateDefaultClient(XtTrue);
  *count = client.simple == nullptr? 0: 2;
  return PA_OK;
}

XtFault PulseAudioService::OpenDevice(int32_t index, XtDevice** device) const {
  XtPaSimple client = CreateDefaultClient(index != 0);
  if(client.simple == nullptr)
    return PA_ERR_INVALIDSERVER;
  *device = new PulseAudioDevice(index != 0);
  return PA_OK;
}

XtFault PulseAudioService::OpenDefaultDevice(XtBool output, XtDevice** device) const {
  XtPaSimple client = CreateDefaultClient(output);
  if(client.simple != nullptr)
    *device = new PulseAudioDevice(output);
  return PA_OK;
}

XtCause PulseAudioService::GetFaultCause(XtFault fault) const {
  switch(fault) {
  case XtPaErrFormat: return XtCauseFormat;
  case PA_ERR_BUSY:
  case PA_ERR_EXIST:
  case PA_ERR_KILLED:
  case PA_ERR_NOENTITY: return XtCauseEndpoint;
  case PA_ERR_VERSION:
  case PA_ERR_INVALIDSERVER:
  case PA_ERR_MODINITFAILED:
  case PA_ERR_CONNECTIONREFUSED:
  case PA_ERR_CONNECTIONTERMINATED: return XtCauseService;
  default: return XtCauseUnknown;
  }
}

// ---- device ----

XtFault PulseAudioDevice::ShowControlPanel() {
  return 0;
}

XtFault PulseAudioDevice::GetName(char** name) const {
  *name = strdup(output? "Output": "Input");
  return PA_OK;
}

XtFault PulseAudioDevice::SupportsAccess(XtBool interleaved, XtBool* supports) const {
  *supports = interleaved;
  return PA_OK;
}

XtFault PulseAudioDevice::GetMix(XtMix** mix) const {
  *mix = static_cast<XtMix*>(malloc(sizeof(XtMix)));
  (*mix)->rate = XtPaDefaultRate;
  (*mix)->sample = XtPaDefaultSample;
  return PA_OK;
}

XtFault PulseAudioDevice::GetChannelCount(XtBool output, int32_t* count) const {
  *count = this->output != output? 0: PA_CHANNEL_POSITION_MAX;
  return PA_OK;
}

XtFault PulseAudioDevice::GetBufferSize(const XtFormat* format, XtBufferSize* size) const {  
  size->min = XtPaMinBufferSize;
  size->max = XtPaMaxBufferSize;
  size->current = XtPaDefaultBufferSize;
  return PA_OK;
}

XtFault PulseAudioDevice::GetChannelName(XtBool output, int32_t index, char** name) const {
  *name = strdup(pa_channel_position_to_pretty_string(static_cast<pa_channel_position_t>(index)));
  return PA_OK;
}

XtFault PulseAudioDevice::SupportsFormat(const XtFormat* format, XtBool* supports) const {
  pa_sample_format pulse;
  if(format->channels.inputs > 0 && output)
    return PA_OK;
  if(format->channels.outputs > 0 && !output)
    return PA_OK;
  if(format->mix.rate < XtPaMinRate)
    return PA_OK;
  if(format->mix.rate > XtPaMaxRate)
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

XtFault PulseAudioDevice::OpenStream(const XtFormat* format, XtBool interleaved, double bufferSize,
                                bool secondary, XtStreamCallback callback, void* user, XtStream** stream) {

  uint64_t mask;
  pa_simple* client;
  int fault = PA_OK;
  int32_t frameSize;
  pa_channel_map map;
  pa_sample_spec spec;
  int32_t bufferFrames;
  std::unique_ptr<PulseAudioStream> result;

  bufferFrames = static_cast<int32_t>(bufferSize / 1000.0 * format->mix.rate);
  spec.rate = format->mix.rate;
  spec.format = ToPulseAudioSample(format->mix.sample);
  spec.channels = format->channels.inputs + format->channels.outputs;
  mask = format->channels.inMask? format->channels.inMask: format->channels.outMask;
  if(mask == 0)
    pa_channel_map_init_extend(&map, spec.channels, PA_CHANNEL_MAP_DEFAULT);
  else {
    pa_channel_map_init(&map);
    map.channels = spec.channels;
    for(int32_t i = 0, j = 0; i < 64; i++)
      if(mask & (1ULL << i))
        map.map[j++] = static_cast<pa_channel_position_t>(i);
  }
  
  frameSize = (format->channels.inputs + format->channels.outputs) * XtiGetSampleSize(format->mix.sample);
  auto dir = output? PA_STREAM_PLAYBACK: PA_STREAM_RECORD;
  if((client = pa_simple_new(nullptr, XtiId, dir, nullptr, 
    XtiId, &spec, &map, nullptr, &fault)) == nullptr)
    return fault;
  *stream = new PulseAudioStream(secondary, XtPaSimple(client), output, bufferFrames, frameSize);
  return PA_OK;
}

// ---- stream ----

void PulseAudioStream::StopStream() {
}

void PulseAudioStream::StartStream() {
}

XtFault PulseAudioStream::GetFrames(int32_t* frames) const {
  *frames = bufferFrames;
  return PA_OK;
}

XtFault PulseAudioStream::GetLatency(XtLatency* latency) const {
  return PA_OK;
}

void PulseAudioStream::ProcessBuffer(bool prefill) {
  int fault;
  XtTime xtTime;
  XtBuffer xtBuffer;
  void* inData = output? nullptr: &audio[0];
  void* outData = !output? nullptr: &audio[0];

  if(!output && pa_simple_read(client.simple, &audio[0], audio.size(), &fault) < 0) {
    XT_VERIFY_STREAM_CALLBACK(fault);
    return;
  }
  xtBuffer.input = inData;
  xtBuffer.output = outData;
  xtBuffer.frames = bufferFrames;
  xtTime.time = 0;
  xtTime.valid = XtFalse;
  xtTime.position = 0;
  ProcessCallback(inData, outData, bufferFrames, 0.0, 0, XtFalse, 0);
  if(output && pa_simple_write(client.simple, &audio[0], audio.size(), &fault) < 0)
    XT_VERIFY_STREAM_CALLBACK(fault);
}

#endif // XT_DISABLE_PULSE_AUDIO
#endif // __linux__