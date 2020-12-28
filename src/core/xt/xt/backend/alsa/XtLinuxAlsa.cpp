#if 0
#if XT_ENABLE_ALSA
#include <xt/api/private/Platform.hpp>
#include <xt/api/private/Service.hpp>
#include <xt/api/private/Device.hpp>
#include <xt/api/private/Stream.hpp>
#include <xt/private/BlockingStream.hpp>
#include <xt/private/Shared.hpp>
#include <xt/private/Services.hpp>
#include <alsa/asoundlib.h>
#include <vector>
#include <climits>
#include <cstring>

struct AlsaStream: public XtBlockingStream {
  const bool mmap;
  const bool output;
  const AlsaPcm pcm;
  uint64_t processed;
  const bool alsaInterleaved;
  const int32_t bufferFrames;
  std::vector<uint8_t> interleavedAudio;
  std::vector<void*> nonInterleavedAudio;
  std::vector<std::vector<uint8_t>> nonInterleavedAudioChannels;  
  XT_IMPLEMENT_BLOCKING_STREAM(ALSA);

  ~AlsaStream() { }
  AlsaStream(AlsaPcm&& p, bool output, bool mmap, bool alsaInterleaved, 
             int32_t bufferFrames, int32_t channels, int32_t sampleSize):
  XtBlockingStream(),
  mmap(mmap), 
  output(output),
  pcm(std::move(p)), 
  processed(0), 
  alsaInterleaved(alsaInterleaved),
  bufferFrames(bufferFrames),
  interleavedAudio(bufferFrames * channels * sampleSize, 0),
  nonInterleavedAudio(channels, nullptr), 
  nonInterleavedAudioChannels(channels, std::vector<uint8_t>(bufferFrames * sampleSize, 0)) {
    for(size_t i = 0; i < channels; i++)
      nonInterleavedAudio[i] = &(nonInterleavedAudioChannels[i][0]);
  }
};

XtFault AlsaDevice::OpenStreamCore(const XtDeviceStreamParams* params, void* user, XtStream** stream) {
  
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
  snd_pcm_stream_t direction = params->format.channels.inputs > 0? SND_PCM_STREAM_CAPTURE: SND_PCM_STREAM_PLAYBACK;

  snd_pcm_hw_params_alloca(&hwParams);
  XT_VERIFY_ALSA(snd_pcm_open(&pcm, info.name.c_str(), direction, 0));
  AlsaPcm alsaPcm(pcm);
  XT_VERIFY_ALSA(snd_pcm_hw_params_any(pcm, hwParams));
  {
    AlsaLogDisabler disabler;
    XT_VERIFY_ALSA(IsAlsaInterleaved(pcm, hwParams, params->stream.interleaved, &alsaInterleaved));
    XT_VERIFY_ALSA(SetHwParams(*this, pcm, hwParams, info.mmap, alsaInterleaved, &params->format, &minBuffer, &maxBuffer));
  }

  realBuffer = params->bufferSize / 1000.0 * params->format.mix.rate;
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

  sampleSize = XtiGetSampleSize(params->format.mix.sample);
  channels = params->format.channels.inputs + params->format.channels.outputs;
  *stream = new AlsaStream(std::move(alsaPcm), info.output, info.mmap, alsaInterleaved, realBuffer, channels, sampleSize);
  return 0;
}

// ---- stream ----

void AlsaStream::StopStream() {
  
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
            OnXRun();
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
        OnBuffer(&xtBuffer);
      } else {
        if((sframes = snd_pcm_readn(pcm.pcm, &nonInterleavedAudio[0], bufferFrames)) < 0) {
          if(sframes == -EPIPE)
            OnXRun();
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
        OnBuffer(&xtBuffer);
      }
    } else {
      if(alsaInterleaved) {        
        xtBuffer.input = nullptr;
        xtBuffer.output = &interleavedAudio[0];
        xtBuffer.frames = bufferFrames;
        xtBuffer.time = time;
        xtBuffer.position = position;
        xtBuffer.timeValid = timeValid;
        OnBuffer(&xtBuffer);
        if((sframes = snd_pcm_writei(pcm.pcm, &interleavedAudio[0], bufferFrames)) < 0) {
          if(sframes == -EPIPE)
            OnXRun();
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
        OnBuffer(&xtBuffer);
        if((sframes = snd_pcm_writen(pcm.pcm, &nonInterleavedAudio[0], bufferFrames)) < 0) {
          if(sframes == -EPIPE)
            OnXRun();
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
      data = static_cast<uint8_t*>(areas[0].addr) + areas[0].first / 8 + offset * areas[0].step / 8;
    else {
      data = &nonInterleavedAudio[0];
      for(c = 0; c < _params.format.channels.inputs + _params.format.channels.outputs; c++) {
        nonInterleavedAudio[c] = static_cast<uint8_t*>(areas[c].addr) + areas[c].first / 8 + offset * areas[c].step / 8;
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
      OnBuffer(&xtBuffer);
    }
    else
    {
      xtBuffer.input = nullptr;
      xtBuffer.output = data;
      xtBuffer.frames = uframes;
      xtBuffer.time = time;
      xtBuffer.position = position;
      xtBuffer.timeValid = timeValid;
      OnBuffer(&xtBuffer);
    }
    fault = snd_pcm_mmap_commit(pcm.pcm, offset, uframes);
    if(fault >= 0 && fault != uframes)
      OnXRun();
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

#endif // XT_ENABLE_ALSA
#endif