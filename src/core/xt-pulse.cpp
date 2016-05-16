#ifdef __linux__
#include "xt-linux.hpp"
#include <memory>
#include <vector>
#include <pulse/simple.h>
#include <pulse/pulseaudio.h>

/* Copyright (C) 2015-2016 Sjoerd van Kreel.
 *
 * This file is part of XT-Audio.
 *
 * XT-Audio is free software: you can redistribute it and/or modify it under the 
 * terms of the GNU Lesser General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later version.
 *
 * XT-Audio is distributed in the hope that it will be useful, but WITHOUT ANY 
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with XT-Audio. If not, see<http://www.gnu.org/licenses/>.
 */

// ---- local ----

static const int32_t XtPaMinRate = 1;
static const int32_t XtPaMaxRate = 192000;
static const int32_t XtPaDefaultRate = 44100;
static const int32_t XtPaDefaultChannels = 2;
static const double XtPaMinBufferSize = 80.0;
static const double XtPaMaxBufferSize = 2000.0;
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

XT_DECLARE_SERVICE(Pulse);

struct PulseDevice: public XtDevice {
  const bool output;
  XT_IMPLEMENT_DEVICE(Pulse);
  PulseDevice(bool output):
  output(output) {}
};

struct PulseStream: public XtlLinuxStream {
  const bool output;
  const XtPaSimple client;
  std::vector<char> audio;
  const int32_t bufferFrames;
  XT_IMPLEMENT_STREAM(Pulse);

  ~PulseStream() { Stop(); }
  PulseStream(XtPaSimple&& c, bool output, int32_t bufferFrames, int32_t frameSize):
  output(output), client(std::move(c)), 
  audio(static_cast<size_t>(bufferFrames * frameSize), '\0'),
  bufferFrames(bufferFrames) 
  { XT_ASSERT(client.simple != nullptr); }

  void StopStream();
  void StartStream();
  void ProcessBuffer(bool prefill); 
};

// ---- local ----

static pa_sample_format ToPulseSample(XtSample sample) {
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
  spec.format = ToPulseSample(XtPaDefaultSample);
  auto dir = output? PA_STREAM_PLAYBACK: PA_STREAM_RECORD;
  return XtPaSimple(pa_simple_new(nullptr, XtiId, dir,
    nullptr, XtiId, &spec, nullptr, nullptr, nullptr));
}

// ---- service ----

const char* PulseService::GetName() const {
  return "PulseAudio";
}

XtFault PulseService::GetFormatFault() const {
  return XtPaErrFormat;
}

XtCapabilities PulseService::GetCapabilities() const {
  return XtCapabilitiesChannelMask;
}

const char* PulseService::GetFaultText(XtFault fault) const {
  return fault == XtPaErrFormat? "XtPaErrFormat": pa_strerror(fault);
}

XtFault PulseService::GetDeviceCount(int32_t* count) const {
  XtPaSimple client = CreateDefaultClient(XtTrue);
  *count = client.simple == nullptr? 0: 2;
  return PA_OK;
}

XtFault PulseService::OpenDevice(int32_t index, XtDevice** device) const {
  XtPaSimple client = CreateDefaultClient(index != 0);
  if(client.simple == nullptr)
    return PA_ERR_INVALIDSERVER;
  *device = new PulseDevice(index != 0);
  return PA_OK;
}

XtFault PulseService::OpenDefaultDevice(XtBool output, XtDevice** device) const {
  XtPaSimple client = CreateDefaultClient(output);
  if(client.simple != nullptr)
    *device = new PulseDevice(output);
  return PA_OK;
}

XtCause PulseService::GetFaultCause(XtFault fault) const {
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

XtFault PulseDevice::ShowControlPanel() {
  return 0;
}

XtFault PulseDevice::GetName(char** name) const {
  *name = strdup(output? "Output": "Input");
  return PA_OK;
}

XtFault PulseDevice::SupportsAccess(XtBool interleaved, XtBool* supports) const {
  *supports = interleaved;
  return PA_OK;
}

XtFault PulseDevice::GetMix(XtMix** mix) const {
  *mix = static_cast<XtMix*>(malloc(sizeof(XtMix)));
  (*mix)->rate = XtPaDefaultRate;
  (*mix)->sample = XtPaDefaultSample;
  return PA_OK;
}

XtFault PulseDevice::GetChannelCount(XtBool output, int32_t* count) const {
  *count = this->output != output? 0: PA_CHANNEL_POSITION_MAX;
  return PA_OK;
}

XtFault PulseDevice::GetBuffer(const XtFormat* format, XtBuffer* buffer) const {  
  buffer->min = XtPaMinBufferSize;
  buffer->max = XtPaMaxBufferSize;
  buffer->current = XtPaMinBufferSize;
  return PA_OK;
}

XtFault PulseDevice::GetChannelName(XtBool output, int32_t index, char** name) const {
  *name = strdup(pa_channel_position_to_pretty_string(static_cast<pa_channel_position_t>(index)));
  return PA_OK;
}

XtFault PulseDevice::SupportsFormat(const XtFormat* format, XtBool* supports) const {
  pa_sample_format pulse;
  if(format->inputs > 0 && output)
    return PA_OK;
  if(format->outputs > 0 && !output)
    return PA_OK;
  if(format->mix.rate < XtPaMinRate)
    return PA_OK;
  if(format->mix.rate > XtPaMaxRate)
    return PA_OK;
  if(format->inputs >= PA_CHANNEL_POSITION_MAX)
    return PA_OK;
  if(format->outputs >= PA_CHANNEL_POSITION_MAX)
    return PA_OK;
  for(int32_t i = PA_CHANNEL_POSITION_MAX; i < 64; i++)
    if(format->inMask & (1ULL << i) || format->outMask & (1ULL << i))
      return PA_OK;
  *supports = XtTrue;
  return PA_OK;
}

XtFault PulseDevice::OpenStream(const XtFormat* format, double bufferSize, XtStreamCallback callback, void* user, XtStream** stream) {

  uint64_t mask;
  pa_simple* client;
  int fault = PA_OK;
  int32_t frameSize;
  pa_channel_map map;
  pa_sample_spec spec;
  int32_t bufferFrames;
  std::unique_ptr<PulseStream> result;

  bufferFrames = static_cast<int32_t>(bufferSize / 1000.0 * format->mix.rate);
  spec.rate = format->mix.rate;
  spec.format = ToPulseSample(format->mix.sample);
  spec.channels = format->inputs + format->outputs;
  mask = format->inMask? format->inMask: format->outMask;
  if(mask == 0)
    pa_channel_map_init_extend(&map, spec.channels, PA_CHANNEL_MAP_DEFAULT);
  else {
    pa_channel_map_init(&map);
    map.channels = spec.channels;
    for(int32_t i = 0, j = 0; i < 64; i++)
      if(mask & (1ULL << i))
        map.map[j++] = static_cast<pa_channel_position_t>(i);
  }
  
  frameSize = (format->inputs + format->outputs) * XtiGetSampleSize(format->mix.sample);
  auto dir = output? PA_STREAM_PLAYBACK: PA_STREAM_RECORD;
  if((client = pa_simple_new(nullptr, XtiId, dir, nullptr, 
    XtiId, &spec, &map, nullptr, &fault)) == nullptr)
    return fault;
  *stream = new PulseStream(XtPaSimple(client), output, bufferFrames, frameSize);
  return PA_OK;
}

// ---- stream ----

void PulseStream::StopStream() {
}

void PulseStream::StartStream() {
}

XtFault PulseStream::GetFrames(int32_t* frames) const {
  *frames = bufferFrames;
  return PA_OK;
}

XtFault PulseStream::GetLatency(XtLatency* latency) const {
  return PA_OK;
}

void PulseStream::ProcessBuffer(bool prefill) {
  int fault;
  void* inData = output? nullptr: &audio[0];
  void* outData = !output? nullptr: &audio[0];

  if(!output && pa_simple_read(client.simple, &audio[0], audio.size(), &fault) < 0) {
    XT_VERIFY_STREAM_CALLBACK(fault);
    return;
  }
  callback(this, inData, outData, bufferFrames, 0.0, 0, XtFalse, 0, user);
  if(output && pa_simple_write(client.simple, &audio[0], audio.size(), &fault) < 0)
    XT_VERIFY_STREAM_CALLBACK(fault);
}

#endif // __linux__
