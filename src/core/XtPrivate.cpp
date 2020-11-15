#include "XtPrivate.hpp"
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

char* XtiId = nullptr;
XtOnError XtiOnError = nullptr;

int32_t XtiGetPopCount64(uint64_t x) {
  const uint64_t m1 = 0x5555555555555555;
  const uint64_t m2 = 0x3333333333333333;
  const uint64_t m4 = 0x0f0f0f0f0f0f0f0f;
  const uint64_t h01 = 0x0101010101010101;
  x -= (x >> 1) & m1;
  x = (x & m2) + ((x >> 2) & m2);
  x = (x + (x >> 4)) & m4;
  return (x * h01) >> 56;
}

int32_t XtiGetSampleSize(XtSample sample) {
  return XtAudioGetSampleAttributes(sample).size;
}

XtError XtiCreateError(XtSystem system, XtFault fault) {
  if(fault == 0)
    return 0;
  auto result = static_cast<XtError>(system) << 32ULL | fault;
  auto info = XtAudioGetErrorInfo(result);
  XT_TRACE(XtPrintErrorInfoToString(&info));
  return result;
}

bool XtiValidateFormat(XtSystem system, const XtFormat& format) {
  const XtService* service = XtAudioGetService(system);
  if(format.channels.inputs < 0 || format.channels.outputs < 0)
    return false;
  if(format.channels.inputs == 0 && format.channels.outputs == 0)
    return false;
  if(format.mix.rate < 1 || format.mix.rate > 384000)
    return false;
  if(format.channels.inMask != 0 && format.channels.inputs != XtiGetPopCount64(format.channels.inMask))
    return false;
  if(format.channels.outMask != 0 && format.channels.outputs != XtiGetPopCount64(format.channels.outMask))
    return false;
  if(format.mix.sample < XtSampleUInt8 || format.mix.sample > XtSampleFloat32)
    return false;
  return true;
}

void XtiFail(const char* file, int line, const char* func, const char* message) {
  XtiTrace(file, line, func, message);
  std::abort();
}

void XtiTrace(const char* file, int32_t line, const char* func, const char* format, ...) {
  va_list arg;
  va_start(arg, format);
  XtiVTrace(file, line, func, format, arg);
  va_end(arg);
}

void XtiVTrace(const char* file, int32_t line, const char* func, const char* format, va_list arg) {
  
  va_list argCopy;
  va_copy(argCopy, arg);
  std::ostringstream location;
  location << file << ":" << line << ": in function " << func;
  int size = vsnprintf(nullptr, 0, format, arg);
  if(size > 0) {
    std::vector<char> message(static_cast<size_t>(size + 1), '\0');
    vsnprintf(&message[0], size + 1, format, argCopy);
    if(XtiOnError != nullptr)
      XtiOnError(location.str().c_str(), message.data());
  }
  va_end(argCopy);
}

uint32_t XtiGetErrorFault(XtError error) {
  return static_cast<XtFault>(error & 0x00000000FFFFFFFF);
}

void XtiOutputString(const char* source, char* buffer, int32_t* size) {
  if(buffer == nullptr) {
    *size = strlen(source) + 1;
    return;
  }
  memcpy(buffer, source, static_cast<size_t>(*size) - 1);
  buffer[*size - 1] = '\0';
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