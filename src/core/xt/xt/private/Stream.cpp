#include <xt/private/Stream.hpp>
#include <xt/private/Shared.hpp>

void
XtStream::OnXRun() const
{
  auto xRun = _params.stream.onXRun;
  if(xRun != nullptr) xRun(-1, _user);
}

void
XtStream::OnBuffer(XtBuffer const* buffer)
{
  XtBuffer converted = *buffer;
  auto onBuffer = _params.stream.onBuffer;
  int32_t inputs = _params.format.channels.inputs;
  int32_t outputs = _params.format.channels.outputs;
  auto interleavedIn = &_buffers.input.interleaved[0];
  auto interleavedOut = &_buffers.output.interleaved[0];
  auto nonInterleavedIn = &_buffers.input.nonInterleaved[0];
  auto nonInterleavedOut = &_buffers.output.nonInterleaved[0];
  bool haveInput = buffer->input != nullptr && buffer->frames > 0;
  bool haveOutput = buffer->output != nullptr && buffer->frames > 0;
  auto nonInterleavedBufferOut = static_cast<void**>(buffer->output);
  auto nonInterleavedBufferIn = static_cast<const void* const*>(buffer->input);   
  int32_t size = XtiGetSampleSize(_params.format.mix.sample);

  if(buffer->error != 0)
  {
    onBuffer(this, buffer, _user);
    return;
  } else if(!_emulated) 
  {
    converted.input = haveInput? buffer->input: nullptr;
    converted.output = haveOutput? buffer->output: nullptr;
    onBuffer(this, &converted, _user);
  } else if(!_params.stream.interleaved) 
  {
    converted.input = haveInput? nonInterleavedIn: nullptr;
    converted.output = haveOutput? nonInterleavedOut: nullptr;
    if(haveInput) XtiDeinterleave(nonInterleavedIn, buffer->input, buffer->frames, inputs, size);
    onBuffer(this, &converted, _user);
    if(haveOutput) XtiInterleave(buffer->output, nonInterleavedOut, buffer->frames, outputs, size);
  } else
  {
    converted.input = haveInput? interleavedIn: nullptr;
    converted.output = haveOutput? interleavedOut: nullptr;
    if(haveInput) XtiInterleave(interleavedIn, nonInterleavedBufferIn, buffer->frames, inputs, size);
    onBuffer(this, &converted, _user);
    if(haveOutput) XtiDeinterleave(nonInterleavedBufferOut, interleavedOut, buffer->frames, outputs, size);
  }
}