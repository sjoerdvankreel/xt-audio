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
AlsaStream::BlockMasterBuffer(XtBool* ready)
{ 
  snd_pcm_sframes_t available;
  if((available = snd_pcm_avail(_pcm.pcm)) < 0) return available;
  *ready = available > 0;
  return 0;
}

XtFault 
AlsaStream::ProcessBuffer()
{
  snd_timestamp_t stamp;
  XtBuffer buffer = { 0 };
  snd_pcm_status_t* status;
  snd_pcm_sframes_t available;

  snd_pcm_status_alloca(&status);
  XT_VERIFY_ALSA(snd_pcm_status(_pcm.pcm, status));
  snd_pcm_status_get_tstamp(status, &stamp);
  buffer.position = _processed;
  buffer.timeValid = stamp.tv_sec != 0 || stamp.tv_usec != 0;
  buffer.time = stamp.tv_sec * 1000.0 + stamp.tv_usec / 1000.0;

  if((available = snd_pcm_avail(_pcm.pcm)) < 0) return available;

  return 0;
}

#endif // XT_ENABLE_ALSA