#ifdef __linux__
#include "XtLinux.hpp"

#ifdef XT_DISABLE_ALSA
void XtlInitAlsa() { }
void XtlTerminateAlsa() { }
const XtService* XtiServiceAlsa = nullptr;
#else // XT_DISABLE_ALSA

#include <vector>
#include <cstring>
#include <climits>
#include <alsa/asoundlib.h>

#define XT_VERIFY_ALSA(c) do { int e = (c); if(e < 0) \
  return XT_TRACE("%s", #c), e; } while(0)

// ---- local ----

static void LogError(const char *file, int line, 
  const char *fun, int err, const char *fmt, ...);
static void LogNull(const char *file, int line, 
  const char *fun, int err, const char *fmt, ...) {}

static const double XtAlsaMinBufferMs = 1.0;
static const double XtAlsaMaxBufferMs = 2000.0;
static const size_t XtAlsaDefaultBufferBytes = 64 * 1024;

struct AlsaDeviceInfo {
  bool output;
  bool mmap;
  std::string name;
  std::string description;
};

struct AlsaLogDisabler {
  AlsaLogDisabler()
  { XT_ASSERT(snd_lib_error_set_handler(&LogNull) == 0); }
  ~AlsaLogDisabler()
  { XT_ASSERT(snd_lib_error_set_handler(&LogError) == 0); }
};

struct AlsaPcm {
  snd_pcm_t* pcm;
  AlsaPcm(const AlsaPcm&) = delete;
  AlsaPcm& operator=(const AlsaPcm&) = delete;

  AlsaPcm(snd_pcm_t* pcm): pcm(pcm)
  { XT_ASSERT(pcm != nullptr); }
  
  AlsaPcm(AlsaPcm&& rhs):
  pcm(rhs.pcm) { rhs.pcm = nullptr; }
  
  AlsaPcm& operator=(AlsaPcm&& rhs)
  { pcm = rhs.pcm; rhs.pcm = nullptr; return *this; }

  ~AlsaPcm()
  { XT_ASSERT(pcm == nullptr || snd_pcm_close(pcm) == 0); }
};

struct AlsaHint {
  char* hint;
  ~AlsaHint() { free(hint); }
  AlsaHint(char* hint): hint(hint) {}
};

struct AlsaHints {
  void** hints;
  ~AlsaHints()
  { XT_ASSERT(snd_device_name_free_hint(hints) == 0); }
  AlsaHints(): hints(nullptr) 
  { XT_ASSERT(snd_device_name_hint(-1, "pcm", &hints) == 0); }
};

// ---- forward ----

XT_DECLARE_SERVICE(ALSA, Alsa);

struct AlsaDevice: public XtDevice {
  const AlsaDeviceInfo info;
  XT_IMPLEMENT_DEVICE(ALSA);

  AlsaDevice(const AlsaDeviceInfo& info): info(info) {}
  XtFault SupportsAccess(snd_pcm_t* pcm, snd_pcm_hw_params_t* hwParams, 
                         XtBool interleaved, XtBool* supports) const;
  XtFault IsAlsaInterleaved(snd_pcm_t* pcm, snd_pcm_hw_params_t* hwParams, 
                            XtBool interleaved, XtBool* alsaInterleaved) const;
};

struct AlsaStream: public XtlLinuxBlockingStream {
  const bool mmap;
  const bool output;
  const AlsaPcm pcm;
  uint64_t processed;
  const bool alsaInterleaved;
  const int32_t bufferFrames;
  std::vector<char> interleavedAudio;
  std::vector<void*> nonInterleavedAudio;
  std::vector<std::vector<char>> nonInterleavedAudioChannels;  
  XT_IMPLEMENT_BLOCKING_STREAM(ALSA);

  ~AlsaStream() { Stop(); }
  AlsaStream(bool secondary, AlsaPcm&& p, bool output, bool mmap, bool alsaInterleaved, 
             int32_t bufferFrames, int32_t channels, int32_t sampleSize):
  XtlLinuxBlockingStream(secondary),
  mmap(mmap), 
  output(output),
  pcm(std::move(p)), 
  processed(0), 
  alsaInterleaved(alsaInterleaved),
  bufferFrames(bufferFrames),
  interleavedAudio(bufferFrames * channels * sampleSize, '\0'),
  nonInterleavedAudio(channels, nullptr), 
  nonInterleavedAudioChannels(channels, std::vector<char>(bufferFrames * sampleSize, '\0')) {
    for(size_t i = 0; i < channels; i++)
      nonInterleavedAudio[i] = &(nonInterleavedAudioChannels[i][0]);
  }
};

// ---- local ----

static void LogError(const char *file, int line, 
  const char *fun, int err, const char *fmt, ...) {

  va_list arg;
  va_start(arg, fmt);
  if(err != 0)
    XtiVTrace(file, line, fun, fmt, arg);
  va_end(arg);
}

static snd_pcm_format_t ToAlsaSample(XtSample sample) {
  switch(sample) {
  case XtSampleUInt8: return SND_PCM_FORMAT_U8; 
  case XtSampleInt16: return SND_PCM_FORMAT_S16_LE; 
  case XtSampleInt24: return SND_PCM_FORMAT_S24_3LE; 
  case XtSampleInt32: return SND_PCM_FORMAT_S32_LE; 
  case XtSampleFloat32: return SND_PCM_FORMAT_FLOAT_LE;
  default: return XT_FAIL("Unknown sample."), SND_PCM_FORMAT_U8;
  }
}

static std::vector<AlsaDeviceInfo> GetDeviceInfos() {
  AlsaHints hints;
  std::vector<AlsaDeviceInfo> result;
  std::vector<AlsaDeviceInfo> outputs;

  for(size_t i = 0; hints.hints[i] != nullptr; i++) {
    AlsaHint ioid(snd_device_name_get_hint(hints.hints[i], "IOID"));
    AlsaHint name(snd_device_name_get_hint(hints.hints[i], "NAME"));
    if(!strcmp("null", name.hint))
      continue;

    if(strstr(name.hint, "dmix") != name.hint 
      && (ioid.hint == nullptr || !strcmp("Input", ioid.hint))) {

      AlsaDeviceInfo device = AlsaDeviceInfo();
      device.name = name.hint;
      device.output = false;
      device.description = device.name + " (Input) (R/W)";
      result.push_back(device);
      device.mmap = true;
      device.description = device.name + " (Input) (MMap)";
      result.push_back(device);
    }

    if(strstr(name.hint, "dsnoop") != name.hint 
      && (ioid.hint == nullptr || !strcmp("Output", ioid.hint))) {

      AlsaDeviceInfo device = AlsaDeviceInfo();
      device.name = name.hint;
      device.output = true;
      device.description = device.name + " (Output) (R/W)";
      outputs.push_back(device);
      device.mmap = true;
      device.description = device.name + " (Output) (MMap)";
      outputs.push_back(device);
    }
  }
  result.insert(result.end(), outputs.begin(), outputs.end());
  return result;
}

static int SetHwParams(const AlsaDevice& device, snd_pcm_t* pcm, snd_pcm_hw_params_t* hwParams, bool mmap, bool interleaved,
                       const XtFormat* format, snd_pcm_uframes_t* minBuffer, snd_pcm_uframes_t* maxBuffer) {

  int fault;
  XtBool alsaInterleaved;
  int32_t channels = format->channels.inputs + format->channels.outputs;
  unsigned urate = static_cast<unsigned>(format->mix.rate);
  snd_pcm_format_t alsaFormat = ToAlsaSample(format->mix.sample);

  if((fault = device.IsAlsaInterleaved(pcm, hwParams, interleaved, &alsaInterleaved)) < 0)
    return fault;
  if((fault = snd_pcm_hw_params_set_rate(pcm, hwParams, urate, 0)) < 0)
    return fault;
  if((fault = snd_pcm_hw_params_set_rate_resample(pcm, hwParams, 0)) < 0)
    return fault;
  if((fault = snd_pcm_hw_params_set_format(pcm, hwParams, alsaFormat)) < 0)
    return fault;
  if((fault = snd_pcm_hw_params_set_channels(pcm, hwParams, channels)) < 0)
    return fault;
  if(mmap && alsaInterleaved && ((fault = snd_pcm_hw_params_set_access(pcm, hwParams, SND_PCM_ACCESS_MMAP_INTERLEAVED)) < 0))
    return fault;
  if(mmap && !alsaInterleaved && ((fault = snd_pcm_hw_params_set_access(pcm, hwParams, SND_PCM_ACCESS_MMAP_NONINTERLEAVED)) < 0))
    return fault;
  if(!mmap && alsaInterleaved && ((fault = snd_pcm_hw_params_set_access(pcm, hwParams, SND_PCM_ACCESS_RW_INTERLEAVED)) < 0))
    return fault;
  if(!mmap && !alsaInterleaved && ((fault = snd_pcm_hw_params_set_access(pcm, hwParams, SND_PCM_ACCESS_RW_NONINTERLEAVED)) < 0))
    return fault;
  if((fault = snd_pcm_hw_params_get_buffer_size_min(hwParams, minBuffer)) < 0)
    return fault;
  if((fault = snd_pcm_hw_params_get_buffer_size_max(hwParams, maxBuffer)) < 0)
    return fault;
  return 0;
}

static int QueryDevice(const AlsaDevice& device, const XtFormat* format, 
                       bool interleaved, XtBool* supports, double* min, double* max) {

  snd_pcm_t* pcm;
  AlsaLogDisabler disabler;
  snd_pcm_uframes_t minBuffer;
  snd_pcm_uframes_t maxBuffer;
  snd_pcm_hw_params_t* hwParams;
  snd_pcm_stream_t stream = format->channels.inputs > 0? SND_PCM_STREAM_CAPTURE: SND_PCM_STREAM_PLAYBACK;

  *min = 0;
  *max = 0;
  *supports = XtFalse;
  if(snd_pcm_open(&pcm, device.info.name.c_str(), stream, 0) < 0) 
    return 0;
  AlsaPcm alsaPcm(pcm);
  snd_pcm_hw_params_alloca(&hwParams);
  if(snd_pcm_hw_params_any(pcm, hwParams) < 0)
    return 0;
  if(SetHwParams(device, pcm, hwParams, device.info.mmap, interleaved, format, &minBuffer, &maxBuffer) < 0)
    return 0;

  *supports = XtTrue;
  *min = minBuffer * 1000.0 / format->mix.rate;
  *max = maxBuffer * 1000.0 / format->mix.rate;
  if(*min < XtAlsaMinBufferMs)
    *min = XtAlsaMinBufferMs;
  if(*max > XtAlsaMaxBufferMs)
    *max = XtAlsaMaxBufferMs;
  return 0;
}

// ---- linux ----

void XtlInitAlsa() {
  XT_ASSERT(snd_lib_error_set_handler(&LogError) == 0);
}

void XtlTerminateAlsa() {
  XT_ASSERT(snd_lib_error_set_handler(nullptr) == 0);
  XT_ASSERT(snd_config_update_free_global() == 0);
}

// ---- service ----

XtFault AlsaService::GetFormatFault() const {
  return EINVAL;
}

const char* AlsaService::GetFaultText(XtFault fault) const {
  return snd_strerror(fault);
}

XtCause AlsaService::GetFaultCause(XtFault fault) const {
  return XtlPosixErrorToCause(std::abs(static_cast<int>(fault)));
}

XtCapabilities AlsaService::GetCapabilities() const {
  return static_cast<XtCapabilities>(
    XtCapabilitiesTime |
    XtCapabilitiesLatency |
    XtCapabilitiesXRunDetection);
}

XtFault AlsaService::GetDeviceCount(int32_t* count) const {
  *count = static_cast<int32_t>(GetDeviceInfos().size());
  return 0;
}

XtFault AlsaService::OpenDevice(int32_t index, XtDevice** device) const {  
  std::vector<AlsaDeviceInfo> infos(GetDeviceInfos());
  if(index >= static_cast<int32_t>(infos.size()))
    return -EINVAL;
  *device = new AlsaDevice(infos[index]);
  return 0;
}

XtFault AlsaService::OpenDefaultDevice(XtBool output, XtDevice** device) const { 

  int32_t firstIndex = -1;
  int32_t defaultIndex = -1;
  std::vector<AlsaDeviceInfo> infos(GetDeviceInfos());

  for(size_t i = 0; i < infos.size(); i++) {
    if(firstIndex == -1 && infos[i].output == output)
      firstIndex = static_cast<int32_t>(i);
    if(defaultIndex == -1 && infos[i].name == "default (Input) (R/W)" && !infos[i].output && !output) {
      defaultIndex = static_cast<int32_t>(i);
      break;
    }
    if(defaultIndex == -1 && infos[i].name == "default (Output) (R/W)" && infos[i].output && output) {
      defaultIndex = static_cast<int32_t>(i);
      break;
    }
  }
  if(defaultIndex != -1)
    *device = new AlsaDevice(infos[defaultIndex]);
  else if(firstIndex != -1)
    *device = new AlsaDevice(infos[firstIndex]);
  return 0;
}

// ---- device ----

XtFault AlsaDevice::ShowControlPanel() {
  return 0;
}

XtFault AlsaDevice::GetMix(XtMix** mix) const {
  return 0;
}

XtFault AlsaDevice::GetName(char** name) const {
  *name = strdup(info.description.c_str());
  return 0;
}

XtFault AlsaDevice::GetChannelCount(XtBool output, int32_t* count) const {  
  *count = info.output != (output != XtFalse)? 0: SND_CHMAP_LAST;
  return 0;
}

XtFault AlsaDevice::GetChannelName(XtBool output, int32_t index, char** name) const {
  *name = strdup(snd_pcm_chmap_long_name(static_cast<snd_pcm_chmap_position>(index)));
  return 0;
}

XtFault AlsaDevice::SupportsFormat(const XtFormat* format, XtBool* supports) const {
  int fault;
  double min, max;
  if(format->channels.inputs > 0 && info.output || format->channels.outputs > 0 && !info.output)
    return 0;
  return QueryDevice(*this, format, true, supports, &min, &max);
}

XtFault AlsaDevice::GetBufferSize(const XtFormat* format, XtBufferSize* size) const {

  XtBool supports;
  double frameSize = (format->channels.inputs + format->channels.outputs) * XtiGetSampleSize(format->mix.sample);

  XT_VERIFY_ALSA(QueryDevice(*this, format, true, &supports, &size->min, &size->max));
  size->current = (XtAlsaDefaultBufferBytes / frameSize) / format->mix.rate * 1000.0;
  if(size->current < size->min)
    size->current = size->min;
  if(buffer->current > size->max)
    size->current = size->max;
  return 0;
}

XtFault AlsaDevice::SupportsAccess(XtBool interleaved, XtBool* supports) const {
  
  snd_pcm_t* pcm;
  AlsaLogDisabler disabler;
  snd_pcm_hw_params_t* hwParams;
  snd_pcm_hw_params_alloca(&hwParams);
  snd_pcm_stream_t stream = !info.output? SND_PCM_STREAM_CAPTURE: SND_PCM_STREAM_PLAYBACK;

  if(snd_pcm_open(&pcm, info.name.c_str(), stream, 0) < 0) 
    return 0;
  AlsaPcm alsaPcm(pcm);
  XT_VERIFY_ALSA(snd_pcm_hw_params_any(pcm, hwParams));
  return SupportsAccess(pcm, hwParams, interleaved, supports);
}

XtFault AlsaDevice::IsAlsaInterleaved(snd_pcm_t* pcm, snd_pcm_hw_params_t* hwParams, 
                                      XtBool interleaved, XtBool* alsaInterleaved) const {

  XtFault fault;
  XtBool canInterleaved;
  XtBool canNonInterleaved;  
  if((fault = SupportsAccess(pcm, hwParams, XtTrue, &canInterleaved)) != 0)
    return fault;
  if((fault = SupportsAccess(pcm, hwParams, XtFalse, &canNonInterleaved)) != 0)
    return fault;
  *alsaInterleaved = canInterleaved && canNonInterleaved? interleaved: canInterleaved;
  return 0;
}

XtFault AlsaDevice::SupportsAccess(snd_pcm_t* pcm, snd_pcm_hw_params_t* hwParams, XtBool interleaved, XtBool* supports) const {

  *supports = 0;
  (*supports) |= (!info.mmap && interleaved && snd_pcm_hw_params_test_access(pcm, hwParams, SND_PCM_ACCESS_RW_INTERLEAVED) >= 0);
  (*supports) |= (info.mmap && interleaved && snd_pcm_hw_params_test_access(pcm, hwParams, SND_PCM_ACCESS_MMAP_INTERLEAVED) >= 0);
  (*supports) |= (!info.mmap && !interleaved && snd_pcm_hw_params_test_access(pcm, hwParams, SND_PCM_ACCESS_RW_NONINTERLEAVED) >= 0);
  (*supports) |= (info.mmap && !interleaved && snd_pcm_hw_params_test_access(pcm, hwParams, SND_PCM_ACCESS_MMAP_NONINTERLEAVED) >= 0);
  return 0;
}

XtFault AlsaDevice::OpenStream(const XtFormat* format, XtBool interleaved, double bufferSize, 
                               bool secondary, XtOnBuffer onBuffer, void* user, XtStream** stream) {
  
  XtFault fault;
  snd_pcm_t* pcm;
  int32_t channels;
  int32_t sampleSize;
  XtBool alsaInterleaved;
  snd_pcm_uframes_t minBuffer;
  snd_pcm_uframes_t maxBuffer;
  snd_pcm_uframes_t realBuffer;
  snd_pcm_hw_params_t* hwParams;
  snd_pcm_sw_params_t* swParams;
  snd_pcm_stream_t direction = format->channels.inputs > 0? SND_PCM_STREAM_CAPTURE: SND_PCM_STREAM_PLAYBACK;

  snd_pcm_hw_params_alloca(&hwParams);
  XT_VERIFY_ALSA(snd_pcm_open(&pcm, info.name.c_str(), direction, 0));
  AlsaPcm alsaPcm(pcm);
  XT_VERIFY_ALSA(snd_pcm_hw_params_any(pcm, hwParams));
  {
    AlsaLogDisabler disabler;
    XT_VERIFY_ALSA(IsAlsaInterleaved(pcm, hwParams, interleaved, &alsaInterleaved));
    XT_VERIFY_ALSA(SetHwParams(*this, pcm, hwParams, info.mmap, alsaInterleaved, format, &minBuffer, &maxBuffer));
  }

  realBuffer = bufferSize / 1000.0 * format->mix.rate;
  if(realBuffer < minBuffer)
    realBuffer = minBuffer;
  if(realBuffer > maxBuffer)
    realBuffer = maxBuffer;
  XT_VERIFY_ALSA(snd_pcm_hw_params_set_buffer_size_near(pcm, hwParams, &realBuffer));
  XT_VERIFY_ALSA(snd_pcm_hw_params(pcm, hwParams));

  snd_pcm_sw_params_alloca(&swParams);
  XT_VERIFY_ALSA(snd_pcm_sw_params_current(pcm, swParams));
  XT_VERIFY_ALSA(snd_pcm_sw_params_set_tstamp_mode(pcm, swParams, SND_PCM_TSTAMP_ENABLE));
  XT_VERIFY_ALSA(snd_pcm_sw_params(pcm, swParams));

  sampleSize = XtiGetSampleSize(format->mix.sample);
  channels = format->channels.inputs + format->channels.outputs;
  *stream = new AlsaStream(secondary, std::move(alsaPcm), info.output, info.mmap, alsaInterleaved, realBuffer, channels, sampleSize);
  return 0;
}

// ---- stream ----

void AlsaStream::StartStream() {
  processed = 0;
  if(snd_pcm_state(pcm.pcm) == SND_PCM_STATE_PREPARED)
    XT_ASSERT(snd_pcm_start(pcm.pcm) == 0);
}

void AlsaStream::StopStream() {
  if(snd_pcm_state(pcm.pcm) == SND_PCM_STATE_RUNNING) {
    XT_ASSERT(snd_pcm_drop(pcm.pcm) == 0);
    XT_ASSERT(snd_pcm_prepare(pcm.pcm) == 0);
  }
  processed = 0;
}

XtFault AlsaStream::GetFrames(int32_t* frames) const {
  *frames = bufferFrames;
  return 0;
}

XtFault AlsaStream::GetLatency(XtLatency* latency) const {
  snd_pcm_sframes_t delay;
  if(snd_pcm_delay(pcm.pcm, &delay) < 0)
    return 0;
  latency->input = output? 0.0: delay * 1000.0 / format.mix.rate;
  latency->output = !output? 0.0: delay * 1000.0 / format.mix.rate;
  return 0;
}

void AlsaStream::ProcessBuffer(bool prefill) {

  int32_t c;
  int fault;
  void* data;
  double time;
  XtBool timeValid;
  uint64_t position;
  snd_pcm_status_t* status;
  snd_pcm_uframes_t offset;
  snd_pcm_sframes_t sframes;
  snd_pcm_uframes_t uframes;
  snd_timestamp_t timeStamp;
  snd_pcm_sframes_t available;
  const snd_pcm_channel_area_t* areas;
  XtBuffer xtBuffer = { 0 };

  snd_pcm_status_alloca(&status);
  if(!XT_VERIFY_ON_BUFFER(snd_pcm_status(pcm.pcm, status)))
    return;
  if((available = snd_pcm_avail(pcm.pcm)) < 0) {
    XT_VERIFY_ON_BUFFER(available);
    return;
  }
  
  snd_pcm_status_get_tstamp(status, &timeStamp);
  position = processed + available;
  time = timeStamp.tv_sec * 1000.0 + timeStamp.tv_usec / 1000.0;
  timeValid = timeStamp.tv_sec != 0 || timeStamp.tv_usec != 0;

  if(!mmap) {
    if(!output) {
      if(alsaInterleaved) {
        if((sframes = snd_pcm_readi(pcm.pcm, &interleavedAudio[0], bufferFrames)) < 0) {
          if(sframes == -EPIPE)
            ProcessXRun();
          if(!XT_VERIFY_ON_BUFFER(snd_pcm_recover(pcm.pcm, sframes, 1)))
            return;
          if((sframes = snd_pcm_readi(pcm.pcm, &interleavedAudio[0], bufferFrames)) < 0) {
            XT_VERIFY_ON_BUFFER(sframes);
            return;
          }
        }        
        xtBuffer.input = &interleavedAudio[0];
        xtBuffer.output = nullptr;
        xtBuffer.frames = sframes;
        xtBuffer.time = time;
        xtBuffer.position = position;
        xtBuffer.timeValid = timeValid;
        ProcessBuffer(&xtBuffer);
      } else {
        if((sframes = snd_pcm_readn(pcm.pcm, &nonInterleavedAudio[0], bufferFrames)) < 0) {
          if(sframes == -EPIPE)
            ProcessXRun();
          if(!XT_VERIFY_ON_BUFFER(snd_pcm_recover(pcm.pcm, sframes, 1)))
            return;
          if((sframes = snd_pcm_readn(pcm.pcm, &nonInterleavedAudio[0], bufferFrames)) < 0) {
            XT_VERIFY_ON_BUFFER(sframes);
            return;
          }
        }
        xtBuffer.input = &nonInterleavedAudio[0];
        xtBuffer.output = nullptr;
        xtBuffer.frames = sframes;
        xtBuffer.time = time;
        xtBuffer.position = position;
        xtBuffer.timeValid = timeValid;
        ProcessBuffer(&xtBuffer);
      }
    } else {
      if(alsaInterleaved) {        
        xtBuffer.input = nullptr;
        xtBuffer.output = &interleavedAudio[0];
        xtBuffer.frames = bufferFrames;
        xtBuffer.time = time;
        xtBuffer.position = position;
        xtBuffer.timeValid = timeValid;
        ProcessBuffer(&xtBuffer);
        if((sframes = snd_pcm_writei(pcm.pcm, &interleavedAudio[0], bufferFrames)) < 0) {
          if(sframes == -EPIPE)
            ProcessXRun();
          if(!XT_VERIFY_ON_BUFFER(snd_pcm_recover(pcm.pcm, sframes, 1)))
            return;
          if((sframes = snd_pcm_writei(pcm.pcm, &interleavedAudio[0], bufferFrames)) < 0)
            XT_VERIFY_ON_BUFFER(sframes);
        }
      } else {        
        xtBuffer.input = nullptr;
        xtBuffer.output = &nonInterleavedAudio[0];
        xtBuffer.frames = bufferFrames;
        xtBuffer.time = time;
        xtBuffer.position = position;
        xtBuffer.timeValid = timeValid;
        ProcessBuffer(&xtBuffer);
        if((sframes = snd_pcm_writen(pcm.pcm, &nonInterleavedAudio[0], bufferFrames)) < 0) {
          if(sframes == -EPIPE)
            ProcessXRun();
          if(!XT_VERIFY_ON_BUFFER(snd_pcm_recover(pcm.pcm, sframes, 1)))
            return;
          if((sframes = snd_pcm_writen(pcm.pcm, &nonInterleavedAudio[0], bufferFrames)) < 0)
            XT_VERIFY_ON_BUFFER(sframes);
        }
      }
    }
    processed += sframes;
  } else {
    uframes = bufferFrames;
    if((fault = snd_pcm_mmap_begin(pcm.pcm, &areas, &offset, &uframes)) < 0) {
      if(!XT_VERIFY_ON_BUFFER(snd_pcm_recover(pcm.pcm, fault, 1)))
        return;
      uframes = bufferFrames;
      if((fault = snd_pcm_mmap_begin(pcm.pcm, &areas, &offset, &uframes)) < 0) {
        XT_VERIFY_ON_BUFFER(fault);
        return;
      }
    }
    if(alsaInterleaved)
      data = static_cast<char*>(areas[0].addr) + areas[0].first / 8 + offset * areas[0].step / 8;
    else {
      data = &nonInterleavedAudio[0];
      for(c = 0; c < format.channels.inputs + format.channels.outputs; c++) {
        nonInterleavedAudio[c] = static_cast<char*>(areas[c].addr) + areas[c].first / 8 + offset * areas[c].step / 8;
      }
    }
    if(!output)
    {
      xtBuffer.input = data;
      xtBuffer.output = nullptr;
      xtBuffer.frames = uframes;
      xtBuffer.time = time;
      xtBuffer.position = position;
      xtBuffer.timeValid = timeValid;
      ProcessBuffer(&xtBuffer);
    }
    else
    {
      xtBuffer.input = nullptr;
      xtBuffer.output = data;
      xtBuffer.frames = uframes;
      xtBuffer.time = time;
      xtBuffer.position = position;
      xtBuffer.timeValid = timeValid;
      ProcessBuffer(&xtBuffer);
    }
    fault = snd_pcm_mmap_commit(pcm.pcm, offset, uframes);
    if(fault >= 0 && fault != uframes)
      ProcessXRun();
    if(fault < 0) {
      if(!XT_VERIFY_ON_BUFFER(snd_pcm_recover(pcm.pcm, fault, 1)))
        return;
      if((fault = snd_pcm_mmap_commit(pcm.pcm, offset, uframes)) < 0) {
        XT_VERIFY_ON_BUFFER(fault);
        return;
      }
    }
    processed += uframes;
  }
}

#endif // XT_DISABLE_ALSA
#endif // __linux__