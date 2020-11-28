#include <xt/audio/XtAudio.h>
#include <xt/audio/XtPrint.h>
#include <xt/private/Shared.hpp>
#include <xt/private/Platform.hpp>
#include <xt/private/Services.hpp>
#include <xt/Private.hpp>
#include <thread>
#include <sstream>
#include <cstring>

int32_t
XtiGetPopCount64(uint64_t x) 
{
  uint64_t const m1 = 0x5555555555555555;
  uint64_t const m2 = 0x3333333333333333;
  uint64_t const m4 = 0x0f0f0f0f0f0f0f0f;
  uint64_t const h01 = 0x0101010101010101;
  x -= (x >> 1) & m1;
  x = (x & m2) + ((x >> 2) & m2);
  x = (x + (x >> 4)) & m4;
  return (x * h01) >> 56;
}

uint32_t
XtiGetErrorFault(XtError error) 
{ 
  uint64_t result = error & 0x00000000FFFFFFFF;
  return static_cast<XtFault>(result); 
}

int32_t
XtiGetSampleSize(XtSample sample) 
{
  auto attrs = XtAudioGetSampleAttributes(sample);
  return attrs.size;
}

XtServiceError
XtiGetServiceError(XtSystem system, XtFault fault)
{
  switch(system)
  {
  case XtSystemALSA: return XtiGetAlsaError(fault);
  case XtSystemJACK: return XtiGetJackError(fault);
  case XtSystemASIO: return XtiGetAsioError(fault);
  case XtSystemPulse: return XtiGetPulseError(fault);
  case XtSystemWASAPI: return XtiGetWasapiError(fault);
  case XtSystemDSound: return XtiGetDSoundError(fault);
  default: return XT_ASSERT(false), XtServiceError();
  }
}

bool
XtiCalledOnMainThread()
{ 
  auto platform = XtPlatform::instance;
  auto id = std::this_thread::get_id();
  return platform != nullptr && id == platform->_threadId;
}

XtError
XtiCreateError(XtSystem system, XtFault fault) 
{
  if(fault == 0) return 0;
  auto result = static_cast<XtError>(system) << 32ULL | fault;
  auto info = XtAudioGetErrorInfo(result);
  XT_TRACE(XtPrintErrorInfoToString(&info));
  return result;
}

void
XtiFail(char const* file, int32_t line, char const* fun, char const* msg)
{
  XtiTrace(file, line, fun, msg);
  std::terminate();
}

void
XtiTrace(char const* file, int32_t line, char const* fun, char const* msg)
{
  auto platform = XtPlatform::instance;
  if(platform == nullptr || platform->_onError == nullptr) return;
  std::ostringstream location;
  location << file << ":" << line << ": in function " << fun;
  platform->_onError(location.str().c_str(), msg);
}

void
XtiCopyString(char const* source, char* buffer, int32_t* size) 
{
  if(buffer == nullptr) return (*size = static_cast<int32_t>(strlen(source)) + 1), void();
  memcpy(buffer, source, static_cast<size_t>(*size) - 1);
  buffer[*size - 1] = '\0';
}

void
XtiDeinterleave(void** dst, void const* src, int32_t frames, int32_t channels, int32_t size)
{
  uint8_t** d = reinterpret_cast<uint8_t**>(dst);
  uint8_t const* s = static_cast<uint8_t const*>(src);
  for(int32_t f = 0; f < frames; f++)
    for(int32_t c = 0; c < channels; c++)
      memcpy(&d[c][f * size], &s[(f * channels + c) * size], size);
}

void
XtiInterleave(void* dst, void const* const* src, int32_t frames, int32_t channels, int32_t size)
{
  uint8_t* d = static_cast<uint8_t*>(dst);
  uint8_t const* const* s = reinterpret_cast<uint8_t const* const*>(src);
  for(int32_t f = 0; f < frames; f++)
    for(int32_t c = 0; c < channels; c++)
      memcpy(&d[(f * channels + c) * size], &s[c][f * size], size);
}