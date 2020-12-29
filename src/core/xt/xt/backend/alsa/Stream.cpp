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
  snd_pcm_sframes_t available;
  bool mmap = XtiAlsaTypeIsMMap(_type);
  bool output = XtiAlsaTypeIsOutput(_type);

  snd_pcm_status_alloca(&status);
  XT_VERIFY_ALSA(snd_pcm_status(_pcm.pcm, status));
  snd_pcm_status_get_tstamp(status, &stamp);
  buffer.position = _processed;
  buffer.timeValid = stamp.tv_sec != 0 || stamp.tv_usec != 0;
  buffer.time = stamp.tv_sec * 1000.0 + stamp.tv_usec / 1000.0;
  if((available = snd_pcm_avail(_pcm.pcm)) < 0) return available;
  available = std::min(available, static_cast<snd_pcm_sframes_t>(_frames));
  _processed += available;
  std::cout << "av1 = " << available << "\n";

  if(!mmap && output && _alsaInterleaved)
  {        
    buffer.frames = available;
    buffer.output = _alsaBuffers.output.interleaved.data();
    std::cout << "1\n";
    XT_VERIFY_ALSA(OnBuffer(_params.index, &buffer));
    std::cout << "2\n";
    sframes = snd_pcm_writei(_pcm.pcm, buffer.output, available);
    std::cout << "3\n";
    if(sframes >= 0) return 0;
    if(sframes == -EPIPE) OnXRun(_params.index);
    std::cout << "4\n";
    XT_VERIFY_ALSA(snd_pcm_recover(_pcm.pcm, sframes, 1));
    std::cout << "5\n";
    XT_VERIFY_ALSA(snd_pcm_writei(_pcm.pcm, buffer.output, available));
    std::cout << "6\n";
  }

  return 0;
}

#endif // XT_ENABLE_ALSA