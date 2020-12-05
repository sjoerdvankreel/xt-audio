#if XT_ENABLE_PULSE
#include <xt/pulse/Shared.hpp>
#include <pulse/pulseaudio.h>
#include <utility>

void PulseStream::StopStream() { }
void PulseStream::StartStream() { }

XtFault
PulseStream::GetFrames(int32_t* frames) const
{ *frames = _frames; return PA_OK; }
XtFault
PulseStream::GetLatency(XtLatency* latency) const
{ return PA_OK; }

PulseStream::
PulseStream(bool secondary, XtPaSimple&& pa, bool out, int32_t frames, int32_t frameSize):
XtBlockingStream(secondary),
_pa(std::move(pa)), 
_output(out),
_frames(frames), 
_audio(static_cast<size_t>(frames * frameSize), 0)
{ XT_ASSERT(_pa.pa != nullptr); }

void 
PulseStream::ProcessBuffer(bool prefill)
{
  int fault;
  XtBuffer buffer = { 0 };
  void* input = _output? nullptr: _audio.data();
  void* output = !_output? nullptr: _audio.data();
  
  if(!_output && pa_simple_read(_pa.pa, input, _audio.size(), &fault) < 0)
    return XT_VERIFY_ON_BUFFER(fault), void();
  buffer.input = input;
  buffer.output = output;
  buffer.frames = _frames;
  OnBuffer(&buffer);
  if(_output && pa_simple_write(_pa.pa, output, _audio.size(), &fault) < 0)
    XT_VERIFY_ON_BUFFER(fault);
}

#endif // XT_ENABLE_PULSE