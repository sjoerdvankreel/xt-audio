#include <xt/api/private/StreamBase.hpp>
#include <xt/private/Shared.hpp>

void
XtStreamBase::OnXRun(int32_t index) const
{
  auto onXRun = _params.stream.onXRun;
  if(onXRun != nullptr) onXRun(_stream, index, _user);
}

uint32_t
XtStreamBase::OnBuffer(XtBuffer const* buffer)
{
  XtBuffer converted = *buffer;
  auto onBuffer = _params.stream.onBuffer;
  uint32_t result = static_cast<uint32_t>(-1);
  int32_t inputs = _params.format.channels.inputs;
  int32_t outputs = _params.format.channels.outputs;
  auto interleavedIn = _buffers.input.interleaved.data();
  auto interleavedOut = _buffers.output.interleaved.data();
  auto nonInterleavedIn = _buffers.input.nonInterleaved.data();
  auto nonInterleavedOut = _buffers.output.nonInterleaved.data();
  bool haveInput = buffer->input != nullptr && buffer->frames > 0;
  bool haveOutput = buffer->output != nullptr && buffer->frames > 0;
  auto nonInterleavedBufferOut = static_cast<void**>(buffer->output);
  auto nonInterleavedBufferIn = static_cast<const void* const*>(buffer->input);   
  int32_t size = XtiGetSampleSize(_params.format.mix.sample);

  if(!_emulated) 
  {
    converted.input = haveInput? buffer->input: nullptr;
    converted.output = haveOutput? buffer->output: nullptr;
    result = onBuffer(_stream, &converted, _user);
  } else if(!_params.stream.interleaved) 
  {
    converted.input = haveInput? nonInterleavedIn: nullptr;
    converted.output = haveOutput? nonInterleavedOut: nullptr;
    if(haveInput) XtiDeinterleave(nonInterleavedIn, buffer->input, buffer->frames, inputs, size);
    result = onBuffer(_stream, &converted, _user);
    if(haveOutput) XtiInterleave(buffer->output, nonInterleavedOut, buffer->frames, outputs, size);
  } else
  {
    converted.input = haveInput? interleavedIn: nullptr;
    converted.output = haveOutput? interleavedOut: nullptr;
    if(haveInput) XtiInterleave(interleavedIn, nonInterleavedBufferIn, buffer->frames, inputs, size);
    result = onBuffer(_stream, &converted, _user);
    if(haveOutput) XtiDeinterleave(nonInterleavedBufferOut, interleavedOut, buffer->frames, outputs, size);
  }
  return result;
}