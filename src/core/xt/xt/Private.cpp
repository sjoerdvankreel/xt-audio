#include <xt/Private.hpp>
#include <xt/private/Platform.hpp>
#include <vector>
#include <cstdarg>
#include <sstream>
#include <iostream>

// ---- local ----

static void Interleave(
  void* dest, const void* const* source, int32_t frames, int32_t channels, int32_t sampleSize) {

  uint8_t* dst = static_cast<uint8_t*>(dest);
  const uint8_t* const* src = reinterpret_cast<const uint8_t* const*>(source);
  for(int32_t f = 0; f < frames; f++)
    for(int32_t c = 0; c < channels; c++)
      memcpy(&dst[(f * channels + c) * sampleSize], &src[c][f * sampleSize], sampleSize);
}

static void Deinterleave(
  void** dest, const void* source, int32_t frames, int32_t channels, int32_t sampleSize) {

  const uint8_t* src = static_cast<const uint8_t*>(source);
  uint8_t** dst = reinterpret_cast<uint8_t**>(dest);
  for(int32_t f = 0; f < frames; f++)
    for(int32_t c = 0; c < channels; c++)
      memcpy(&dst[c][f * sampleSize], &src[(f * channels + c) * sampleSize], sampleSize);
}
 
// ---- stream ----

XtBlockingStream::XtBlockingStream(bool secondary):
secondary(secondary) {
}

void XtStream::RequestStop() {
  XT_FAIL("Async stop request not supported on the current stream.");
}

void XtStream::OnXRun() {
  if(_params.stream.onXRun == nullptr)
    return;
  if(_internal.aggregated)
    _params.stream.onXRun(_internal.index, static_cast<XtAggregateContext*>(_user)->stream->_user);
  else
    _params.stream.onXRun(0, _user);
}

void XtStream::OnBuffer(const XtBuffer* buffer) {

  if(buffer->error != 0)
  {
    _params.stream.onBuffer(this, buffer, _user);
    return;
  }

  XtBuffer converted = *buffer;
  int32_t sampleSize = XtiGetSampleSize(_params.format.mix.sample);
  bool haveInput = buffer->input != nullptr && buffer->frames > 0;
  bool haveOutput = buffer->output != nullptr && buffer->frames > 0;

  if(!_emulated) {
    converted.input = haveInput? buffer->input: nullptr;
    converted.output = haveOutput? buffer->output: nullptr;
    _params.stream.onBuffer(this, &converted, _user);
  } else if(_params.stream.interleaved) {
    converted.input = haveInput? &_buffers.input.interleaved[0]: nullptr;
    converted.output = haveOutput? &_buffers.output.interleaved[0]: nullptr;
    if(haveInput)
      Interleave(&_buffers.input.interleaved[0], static_cast<const void* const*>(buffer->input), buffer->frames, _params.format.channels.inputs, sampleSize);
    _params.stream.onBuffer(this, &converted, _user);
    if(haveOutput)
      Deinterleave(static_cast<void**>(buffer->output), &_buffers.output.interleaved[0], buffer->frames, _params.format.channels.outputs, sampleSize);
  } else {
    converted.input = haveInput? &_buffers.input.nonInterleaved[0]: nullptr;
    converted.output = haveOutput? &_buffers.output.nonInterleaved[0]: nullptr;
    if(haveInput)
      Deinterleave(&_buffers.input.nonInterleaved[0], buffer->input, buffer->frames, _params.format.channels.inputs, sampleSize);
    _params.stream.onBuffer(this, &converted, _user);
    if(haveOutput)
      Interleave(buffer->output, &_buffers.output.nonInterleaved[0], buffer->frames, _params.format.channels.outputs, sampleSize);
  }
}