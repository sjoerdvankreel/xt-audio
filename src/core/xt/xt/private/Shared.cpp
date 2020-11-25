#include <xt/audio/XtAudio.h>
#include <xt/audio/XtPrint.h>
#include <xt/private/Shared.hpp>
#include <xt/private/Platform.hpp>
#include <xt/private/Services.hpp>
#include <xt/Private.hpp>
#include <thread>
#include <cassert>
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

bool
XtiCalledOnMainThread()
{ 
  auto platform = XtPlatform::instance;
  auto id = std::this_thread::get_id();
  return platform != nullptr && id == platform->threadId;
}

char const* 
XtiGetFaultText(XtSystem system, XtFault fault)
{
  switch(system)
  {
  case XtSystemALSA: return XtiGetAlsaFaultText(fault);
  case XtSystemJACK: return XtiGetJackFaultText(fault);
  case XtSystemASIO: return XtiGetAsioFaultText(fault);
  case XtSystemPulse: return XtiGetPulseFaultText(fault);
  case XtSystemWASAPI: return XtiGetWasapiFaultText(fault);
  case XtSystemDSound: return XtiGetDSoundFaultText(fault);
  default: return assert(false), nullptr;
  }
}

XtCause 
XtiGetFaultCause(XtSystem system, XtFault fault)
{
  switch(system)
  {
  case XtSystemALSA: return XtiGetAlsaFaultCause(fault);
  case XtSystemJACK: return XtiGetJackFaultCause(fault);
  case XtSystemASIO: return XtiGetAsioFaultCause(fault);
  case XtSystemPulse: return XtiGetPulseFaultCause(fault);
  case XtSystemWASAPI: return XtiGetWasapiFaultCause(fault);
  case XtSystemDSound: return XtiGetDSoundFaultCause(fault);
  default: return assert(false), XtCauseUnknown;
  }
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
XtiCopyString(char const* source, char* buffer, int32_t* size) 
{
  if(buffer == nullptr) return (*size = strlen(source) + 1), void();
  memcpy(buffer, source, static_cast<size_t>(*size) - 1);
  buffer[*size - 1] = '\0';
}