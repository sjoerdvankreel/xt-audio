#if XT_ENABLE_PULSE
#include <xt/backend/pulse/Shared.hpp>

#include <pulse/pulseaudio.h>
#include <utility>

void
PulseStream::StopSlaveBuffer() { }
void
PulseStream::StopMasterBuffer() { }
XtFault
PulseStream::StartSlaveBuffer() { return PA_OK; }
XtFault
PulseStream::BlockMasterBuffer() { return PA_OK; }
XtFault
PulseStream::StartMasterBuffer() { return PA_OK; }
XtFault
PulseStream::PrefillOutputBuffer() { return PA_OK; }

XtFault 
PulseStream::ProcessBuffer()
{  
  int fault;
  XtBuffer buffer = { 0 };
  void* input = _output? nullptr: _audio.data();
  void* output = !_output? nullptr: _audio.data();  
  if(!_output && pa_simple_read(_pa.pa, input, _audio.size(), &fault) < 0) return fault;
  buffer.input = input;
  buffer.output = output;
  buffer.frames = _frames;
  if((fault = OnBuffer(_params.index, &buffer)) != 0) return fault;
  if(_output && pa_simple_write(_pa.pa, output, _audio.size(), &fault) < 0) return fault;
  return PA_OK;
}

#endif // XT_ENABLE_PULSE