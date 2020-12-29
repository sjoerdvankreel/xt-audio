#if XT_ENABLE_ALSA
#include <xt/backend/alsa/Shared.hpp>
#include <xt/backend/alsa/Private.hpp>

#include <iostream>

void*
AlsaStream::GetHandle() const
{ return _pcm.pcm; }
XtFault
AlsaStream::PrefillOutputBuffer()
{ return ProcessBuffer(); }
void
AlsaStream::StopMasterBuffer() { }
XtFault
AlsaStream::BlockMasterBuffer(XtBool* ready)
{ *ready = XtTrue; return 0; }
XtFault
AlsaStream::GetFrames(int32_t* frames) const 
{ *frames = _frames; return 0; }
XtFault
AlsaStream::StartMasterBuffer() { return 0; }

void
AlsaStream::StopSlaveBuffer()
{
  _processed = 0;
  XT_TRACE_IF(snd_pcm_drop(_pcm.pcm));
} 

XtFault
AlsaStream::GetLatency(XtLatency* latency) const
{ 
  snd_pcm_sframes_t delay;
  auto rate = _params.format.mix.rate;
  bool output = XtiAlsaTypeIsOutput(_type);
  if(snd_pcm_delay(_pcm.pcm, &delay) < 0) return 0;
  latency->input = output? 0.0: delay * 1000.0 / rate;
  latency->output = !output? 0.0: delay * 1000.0 / rate;
  return 0;
}

XtFault
AlsaStream::StartSlaveBuffer()
{
  _processed = 0;
  bool mmap = XtiAlsaTypeIsMMap(_type);
  auto state = snd_pcm_state(_pcm.pcm);
  if(state != SND_PCM_STATE_PREPARED && state != SND_PCM_STATE_RUNNING)
    XT_VERIFY_ALSA(snd_pcm_prepare(_pcm.pcm));
  if(mmap && state != SND_PCM_STATE_RUNNING)
    XT_VERIFY_ALSA(snd_pcm_start(_pcm.pcm));
  return 0;
}

XtFault 
AlsaStream::ProcessBuffer()
{
  int err;  
  void* data;
  snd_timestamp_t stamp;
  XtBuffer buffer = { 0 };
  snd_pcm_status_t* status;
  snd_pcm_uframes_t offset;
  snd_pcm_sframes_t sframes;
  snd_pcm_uframes_t uframes;  
  snd_pcm_channel_area_t const* areas;
  bool mmap = XtiAlsaTypeIsMMap(_type);
  bool output = XtiAlsaTypeIsOutput(_type);

  snd_pcm_status_alloca(&status);
  XT_VERIFY_ALSA(snd_pcm_status(_pcm.pcm, status));
  snd_pcm_status_get_tstamp(status, &stamp);
  buffer.position = _processed;
  buffer.timeValid = stamp.tv_sec != 0 || stamp.tv_usec != 0;
  buffer.time = stamp.tv_sec * 1000.0 + stamp.tv_usec / 1000.0;
  
  _processed += _frames;
  buffer.frames = _frames;

  if(!mmap && !output && _alsaInterleaved)
  {
    auto alsaBuf = _alsaBuffers.interleaved.data();
    buffer.input = alsaBuf;
    sframes = snd_pcm_readi(_pcm.pcm, alsaBuf, _frames);
    if(sframes >= 0) return OnBuffer(_params.index, &buffer); 
    if(sframes == -EPIPE) OnXRun(_params.index);
    XT_VERIFY_ALSA(snd_pcm_recover(_pcm.pcm, sframes, 1));
    XT_VERIFY_ALSA(snd_pcm_readi(_pcm.pcm, alsaBuf, _frames));
    return OnBuffer(_params.index, &buffer);
  }

  if(!mmap && !output && !_alsaInterleaved)
  {
    auto alsaBuf = _alsaBuffers.nonInterleaved.data();
    buffer.input = alsaBuf;
    auto appBuf = reinterpret_cast<void**>(alsaBuf);
    sframes = snd_pcm_readn(_pcm.pcm, appBuf, _frames);
    if(sframes >= 0) return OnBuffer(_params.index, &buffer); 
    if(sframes == -EPIPE) OnXRun(_params.index);
    XT_VERIFY_ALSA(snd_pcm_recover(_pcm.pcm, sframes, 1));
    XT_VERIFY_ALSA(snd_pcm_readn(_pcm.pcm, appBuf, _frames));
    return OnBuffer(_params.index, &buffer);
  }

  if(!mmap && output && _alsaInterleaved)
  {        
    buffer.output = _alsaBuffers.interleaved.data();
    XT_VERIFY_ALSA(OnBuffer(_params.index, &buffer));
    sframes = snd_pcm_writei(_pcm.pcm, buffer.output, _frames);
    if(sframes >= 0) return 0;
    if(sframes == -EPIPE) OnXRun(_params.index);
    XT_VERIFY_ALSA(snd_pcm_recover(_pcm.pcm, sframes, 1));
    XT_VERIFY_ALSA(snd_pcm_writei(_pcm.pcm, buffer.output, _frames));
    return 0;
  }

  if(!mmap && output && !_alsaInterleaved)
  {        
    buffer.output = _alsaBuffers.nonInterleaved.data();
    auto buf = reinterpret_cast<void**>(buffer.output);
    XT_VERIFY_ALSA(OnBuffer(_params.index, &buffer));
    sframes = snd_pcm_writen(_pcm.pcm, buf, _frames);
    if(sframes >= 0) return 0;
    if(sframes == -EPIPE) OnXRun(_params.index);
    XT_VERIFY_ALSA(snd_pcm_recover(_pcm.pcm, sframes, 1));
    XT_VERIFY_ALSA(snd_pcm_writen(_pcm.pcm, buf, _frames));
    return 0;
  }

  uframes = _frames;
  XT_VERIFY_ALSA(snd_pcm_avail(_pcm.pcm));
  if((err = snd_pcm_mmap_begin(_pcm.pcm, &areas, &offset, &uframes)) < 0)
  {
    if(err == -EPIPE) OnXRun(_params.index);
    XT_VERIFY_ALSA(snd_pcm_recover(_pcm.pcm, err, 1));
    XT_VERIFY_ALSA(snd_pcm_avail_update(_pcm.pcm));
    XT_VERIFY_ALSA(snd_pcm_mmap_begin(_pcm.pcm, &areas, &offset, &uframes));
  }

  std::cout << "uf=*********" << " " << uframes << "\n";

 if(_alsaInterleaved)
      data = static_cast<uint8_t*>(areas[0].addr) + areas[0].first / 8 + offset * areas[0].step / 8;
    else {
      data = &_alsaBuffers.nonInterleaved[0];
      for(auto c = 0; c < _params.format.channels.inputs + _params.format.channels.outputs; c++) {
        _alsaBuffers.nonInterleaved[c] = static_cast<uint8_t*>(areas[c].addr) + areas[c].first / 8 + offset * areas[c].step / 8;
      }
    }

  if(output) buffer.output = data;
  else buffer.input = data;
  XT_VERIFY_ALSA(OnBuffer(_params.index, &buffer));

  err = snd_pcm_mmap_commit(_pcm.pcm, offset, uframes);
  if(err >= 0 && err != uframes) OnXRun(_params.index);
  if(err == uframes) return 0;
  XT_VERIFY_ALSA(snd_pcm_recover(_pcm.pcm, err, 1));
  XT_VERIFY_ALSA(snd_pcm_mmap_commit(_pcm.pcm, offset, uframes));
  return 0;
}

#endif // XT_ENABLE_ALSA