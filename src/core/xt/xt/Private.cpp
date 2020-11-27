#include <xt/Private.hpp>
#include <xt/private/Platform.hpp>
#include <vector>
#include <cstdarg>
#include <sstream>
#include <iostream>

// ---- local ----

static void Interleave(
  void* dest, const void* const* source, int32_t frames, int32_t channels, int32_t sampleSize) {

  char* dst = static_cast<char*>(dest);
  const char* const* src = reinterpret_cast<const char* const*>(source);
  for(int32_t f = 0; f < frames; f++)
    for(int32_t c = 0; c < channels; c++)
      memcpy(&dst[(f * channels + c) * sampleSize], &src[c][f * sampleSize], sampleSize);
}

static void Deinterleave(
  void** dest, const void* source, int32_t frames, int32_t channels, int32_t sampleSize) {

  const char* src = static_cast<const char*>(source);
  char** dst = reinterpret_cast<char**>(dest);
  for(int32_t f = 0; f < frames; f++)
    for(int32_t c = 0; c < channels; c++)
      memcpy(&dst[c][f * sampleSize], &src[(f * channels + c) * sampleSize], sampleSize);
}
 
// ---- internal ----

void XtiFail(const char* file, int line, const char* func, const char* message) {
  XtiTrace(file, line, func, message);
  std::abort();
}

void XtiTrace(const char* file, int32_t line, const char* func, const char* message) {
  auto platform = XtPlatform::instance;
  if(platform == nullptr || platform->onError == nullptr) return;
  std::ostringstream location;
  location << file << ":" << line << ": in function " << func;
  platform->onError(location.str().c_str(), message);
}

// ---- stream ----

XtBlockingStream::XtBlockingStream(bool secondary):
secondary(secondary) {
}

void XtStream::RequestStop() {
  XT_FAIL("Async stop request not supported on the current stream.");
}

void XtStream::OnXRun() {
  if(onXRun == nullptr)
    return;
  if(aggregated)
    onXRun(aggregationIndex, static_cast<XtAggregateContext*>(user)->stream->user);
  else
    onXRun(0, user);
}

void XtStream::OnBuffer(const XtBuffer* buffer) {

  if(buffer->error != 0)
  {
    onBuffer(this, buffer, user);
    return;
  }

  XtBuffer converted = *buffer;
  bool haveInput = buffer->input != nullptr && buffer->frames > 0;
  bool haveOutput = buffer->output != nullptr && buffer->frames > 0;

  if(interleaved && canInterleaved || !interleaved && canNonInterleaved) {
    converted.input = haveInput? buffer->input: nullptr;
    converted.output = haveOutput? buffer->output: nullptr;
    onBuffer(this, &converted, user);
  } else if(interleaved) {
    converted.input = haveInput? &intermediate.inputInterleaved[0]: nullptr;
    converted.output = haveOutput? &intermediate.outputInterleaved[0]: nullptr;
    if(haveInput)
      Interleave(&intermediate.inputInterleaved[0], static_cast<const void* const*>(buffer->input), buffer->frames, format.channels.inputs, sampleSize);
    onBuffer(this, &converted, user);
    if(haveOutput)
      Deinterleave(static_cast<void**>(buffer->output), &intermediate.outputInterleaved[0], buffer->frames, format.channels.outputs, sampleSize);
  } else {
    converted.input = haveInput? &intermediate.inputNonInterleaved[0]: nullptr;
    converted.output = haveOutput? &intermediate.outputNonInterleaved[0]: nullptr;
    if(haveInput)
      Deinterleave(&intermediate.inputNonInterleaved[0], buffer->input, buffer->frames, format.channels.inputs, sampleSize);
    onBuffer(this, &converted, user);
    if(haveOutput)
      Interleave(buffer->output, &intermediate.outputNonInterleaved[0], buffer->frames, format.channels.outputs, sampleSize);
  }
}