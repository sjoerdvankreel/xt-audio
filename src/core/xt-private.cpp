#include "xt-private.hpp"
#include <vector>
#include <cstdarg>
#include <sstream>
#include <iostream>

// ---- local ----

static void Interleave(
  void* dest, void** source, int32_t frames, int32_t channels, int32_t sampleSize) {

  char* dst = static_cast<char*>(dest);
  char** src = reinterpret_cast<char**>(source);
  for(int32_t f = 0; f < frames; f++)
    for(int32_t c = 0; c < channels; c++)
      memcpy(&dst[(f * channels + c) * sampleSize], &src[c][f * sampleSize], sampleSize);
}

static void Deinterleave(
  void** dest, void* source, int32_t frames, int32_t channels, int32_t sampleSize) {

  char* src = static_cast<char*>(source);
  char** dst = reinterpret_cast<char**>(dest);
  for(int32_t f = 0; f < frames; f++)
    for(int32_t c = 0; c < channels; c++)
      memcpy(&dst[c][f * sampleSize], &src[(f * channels + c) * sampleSize], sampleSize);
}
 
// ---- internal ----

char* XtiId = nullptr;
XtTraceCallback XtiTraceCallback = nullptr;
XtFatalCallback XtiFatalCallback = nullptr;

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

std::string XtiTryGetDeviceName(const XtDevice* d) {
  int32_t size;
  if(d == nullptr || XtDeviceGetName(d, nullptr, &size) != 0)    
    return "<unknown>";
  std::vector<char> buffer(static_cast<size_t>(size));
  if(XtDeviceGetName(d, buffer.data(), &size) != 0)
    return "<unknown>";
  return std::string(buffer.data());
}

XtError XtiCreateError(XtSystem system, XtFault fault) {
  if(fault == 0)
    return 0;
  const char* code = XtAudioGetServiceBySystem(system)->GetFaultText(fault);
  XT_TRACE(XtLevelError, "Fault: system %d, code %u (%s).", system, fault, code);
  return static_cast<uint64_t>(system) << 32ULL | fault;
}

bool XtiValidateFormat(XtSystem system, const XtFormat& format) {
  const XtService* service = XtAudioGetServiceBySystem(system);
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
  XtiTrace(XtLevelFatal, file, line, func, message);
  if(XtiFatalCallback)
    XtiFatalCallback();
  std::abort();
}

void XtiTrace(XtLevel level, const char* file, int32_t line, const char* func, const char* format, ...) {
  va_list arg;
  va_start(arg, format);
  XtiVTrace(level, file, line, func, format, arg);
  va_end(arg);
}

void XtiVTrace(XtLevel level, const char* file, int32_t line, const char* func, const char* format, va_list arg) {
  
  va_list argCopy;
  va_copy(argCopy, arg);
  std::ostringstream oss;

  int size = vsnprintf(nullptr, 0, format, arg);
  if(size > 0) {
    std::vector<char> message(static_cast<size_t>(size + 1), '\0');
    vsnprintf(&message[0], size + 1, format, argCopy);
    if(level > XtLevelInfo)
      oss << file << ":" << line << ": in function " << func << ": ";
    oss << &message[0];
    if(XtiTraceCallback)
      XtiTraceCallback(level, oss.str().c_str());
    if(level == XtLevelFatal)
      std::cerr << "XT-Audio: FATAL: " << oss.str().c_str() << std::endl;
  }
  va_end(argCopy);
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

void XtStream::ProcessXRun() {
  if(xRunCallback == nullptr)
    return;
  if(aggregated)
    xRunCallback(aggregationIndex, static_cast<XtAggregateContext*>(user)->stream->user);
  else
    xRunCallback(0, user);
}

void XtStream::ProcessCallback(void* input, void* output, int32_t frames, double time, 
                               uint64_t position, XtBool timeValid, XtError error) {

  void* inData;
  void* outData;
  bool haveInput = input != nullptr && frames > 0;
  bool haveOutput = output != nullptr && frames > 0;

  if(interleaved && canInterleaved || !interleaved && canNonInterleaved) {
    inData = haveInput? input: nullptr;
    outData = haveOutput? output: nullptr;
    streamCallback(this, inData, outData, frames, time, position, timeValid, error, user);
  } else if(interleaved) {
    inData = haveInput? &intermediate.inputInterleaved[0]: nullptr;
    outData = haveOutput? &intermediate.outputInterleaved[0]: nullptr;
    if(haveInput)
      Interleave(&intermediate.inputInterleaved[0], static_cast<void**>(input), frames, format.channels.inputs, sampleSize);
    streamCallback(this, inData, outData, frames, time, position, timeValid, error, user);
    if(haveOutput)
      Deinterleave(static_cast<void**>(output), &intermediate.outputInterleaved[0], frames, format.channels.outputs, sampleSize);
  } else {
    inData = haveInput? &intermediate.inputNonInterleaved[0]: nullptr;
    outData = haveOutput? &intermediate.outputNonInterleaved[0]: nullptr;
    if(haveInput)
      Deinterleave(&intermediate.inputNonInterleaved[0], input, frames, format.channels.inputs, sampleSize);
    streamCallback(this, inData, outData, frames, time, position, timeValid, error, user);
    if(haveOutput)
      Interleave(output, &intermediate.outputNonInterleaved[0], frames, format.channels.outputs, sampleSize);
  }
}