#if XT_ENABLE_ALSA
#include <xt/backend/alsa/Shared.hpp>
#include <xt/backend/alsa/Private.hpp>

void*
AlsaStream::GetHandle() const
{ return _pcm.pcm; }
void
AlsaStream::StopMasterBuffer() { }
XtFault
AlsaStream::GetFrames(int32_t* frames) const 
{ *frames = _frames; return 0; }
XtFault
AlsaStream::BlockMasterBuffer(XtBool* ready)
{ *ready = XtTrue; return 0; }
XtFault
AlsaStream::StartMasterBuffer() { return 0; }

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

void
AlsaStream::StopSlaveBuffer()
{
  _processed = 0;
  if(snd_pcm_state(_pcm.pcm) != SND_PCM_STATE_RUNNING) return;
  XT_TRACE_IF(snd_pcm_drop(_pcm.pcm));
  XT_TRACE_IF(snd_pcm_prepare(_pcm.pcm));
}

XtFault
AlsaStream::StartSlaveBuffer()
{
  _processed = 0;
  if(snd_pcm_state(_pcm.pcm) != SND_PCM_STATE_PREPARED) return 0;
  XT_VERIFY_ALSA(snd_pcm_start(_pcm.pcm));
  return 0;
}

XtFault
AlsaStream::PrefillOutputBuffer()
{
  return 0;
}  

XtFault 
AlsaStream::ProcessBuffer()
{  
  return 0;
}

#endif // XT_ENABLE_ALSA