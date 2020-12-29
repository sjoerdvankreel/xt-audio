#if XT_ENABLE_ALSA
#include <xt/backend/alsa/Shared.hpp>
#include <xt/backend/alsa/Private.hpp>

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
AlsaStream::StartSlaveBuffer()
{
  _processed = 0;
  XT_VERIFY_ALSA(snd_pcm_prepare(_pcm.pcm));
  return 0;
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
AlsaStream::ProcessBuffer()
{
  snd_timestamp_t stamp;
  XtBuffer buffer = { 0 };
  snd_pcm_status_t* status;
  snd_pcm_sframes_t sframes;
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

  if(!mmap && input && _alsaInterleaved)
  {
    buffer.input = _alsaBuffers.input.interleaved.data();
    sframes = snd_pcm_readi(_pcm.pcm, buffer.input, _frames);
    if(sframes >= 0) return OnBuffer(_params.index, &buffer); 
    if(sframes == -EPIPE) OnXRun(_params.index);
    XT_VERIFY_ALSA(snd_pcm_recover(_pcm.pcm, sframes, 1));
    XT_VERIFY_ALSA(snd_pcm_readi(_pcm.pcm, buffer.input, _frames));
    return OnBuffer(_params.index, &buffer);
  }

  if(!mmap && input && !_alsaInterleaved)
  {
    buffer.input = _alsaBuffers.input.nonInterleaved.data();
    auto input = reinterpret_cast<void**>(buffer.input);
    sframes = snd_pcm_readn(_pcm.pcm, input, _frames);
    if(sframes >= 0) return OnBuffer(_params.index, &buffer); 
    if(sframes == -EPIPE) OnXRun(_params.index);
    XT_VERIFY_ALSA(snd_pcm_recover(_pcm.pcm, sframes, 1));
    XT_VERIFY_ALSA(snd_pcm_readn(_pcm.pcm, input, _frames));
    return OnBuffer(_params.index, &buffer);
  }

  if(!mmap && output && _alsaInterleaved)
  {        
    buffer.output = _alsaBuffers.output.interleaved.data();
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
    buffer.output = _alsaBuffers.output.nonInterleaved.data();
    auto output = reinterpret_cast<void**>(buffer.output);
    XT_VERIFY_ALSA(OnBuffer(_params.index, &buffer));
    sframes = snd_pcm_writen(_pcm.pcm, output, _frames);
    if(sframes >= 0) return 0;
    if(sframes == -EPIPE) OnXRun(_params.index);
    XT_VERIFY_ALSA(snd_pcm_recover(_pcm.pcm, sframes, 1));
    XT_VERIFY_ALSA(snd_pcm_writen(_pcm.pcm, output, _frames));
    return 0;
  }

  return 0;
}

#endif // XT_ENABLE_ALSA