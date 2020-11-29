#include <xt/audio/XtPrint.h>
#include <xt/Private.hpp>
#include <string>
#include <cstring>
#include <sstream>
#include <algorithm>

char const* XT_CALL
XtPrintSampleToString(XtSample sample) 
{
  switch(sample) 
  {
  case XtSampleUInt8: return "UInt8";
  case XtSampleInt16: return "Int16";
  case XtSampleInt24: return "Int24";
  case XtSampleInt32: return "Int32";
  case XtSampleFloat32: return "Float32";
  default: XT_ASSERT(false); return nullptr;
  }
}

char const* XT_CALL
XtPrintCauseToString(XtCause cause) 
{
  switch(cause) 
  {
  case XtCauseFormat: return "Format";
  case XtCauseGeneric: return "Generic";
  case XtCauseService: return "Service";
  case XtCauseUnknown: return "Unknown";
  case XtCauseEndpoint: return "Endpoint";
  default: XT_ASSERT(false); return nullptr;
  }
}

char const* XT_CALL 
XtPrintSystemToString(XtSystem system) 
{
  switch(system) 
  {
  case XtSystemALSA: return "ALSA";
  case XtSystemASIO: return "ASIO";
  case XtSystemJACK: return "JACK";
  case XtSystemWASAPI: return "WASAPI";
  case XtSystemPulse: return "PulseAudio";
  case XtSystemDSound: return "DirectSound";
  default: XT_ASSERT(false); return nullptr;
  }
}

char const* XT_CALL
XtPrintSetupToString(XtSetup setup) 
{
  switch(setup) 
  {
  case XtSetupProAudio: return "ProAudio";
  case XtSetupSystemAudio: return "SystemAudio";
  case XtSetupConsumerAudio: return "ConsumerAudio";
  default: XT_ASSERT(false); return nullptr;
  }
}

char const* XT_CALL
XtPrintCapabilitiesToString(XtCapabilities capabilities) 
{
  size_t i = 0;
  std::string result;
  if(capabilities == 0) return "None";
  static thread_local char buffer[128];
  if((capabilities & XtCapabilitiesTime) != 0) result += "Time, ";
  if((capabilities & XtCapabilitiesLatency) != 0) result += "Latency, ";
  if((capabilities & XtCapabilitiesFullDuplex) != 0) result += "FullDuplex, ";
  if((capabilities & XtCapabilitiesChannelMask) != 0) result += "ChannelMask, ";
  if((capabilities & XtCapabilitiesXRunDetection) != 0) result += "XRunDetection, ";
  std::memcpy(buffer, result.data(), result.size() - 2);
  buffer[result.size() - 2] = '\0';
  return buffer;
}

char const* XT_CALL
XtPrintLocationToString(XtLocation const* location) 
{
  std::ostringstream stream;
  static thread_local char buffer[1024];
  std::memset(buffer, 0, sizeof(buffer));
  stream << location->file << ":" << location->line << ": in function " << location->func;
  auto result = stream.str();
  std::memcpy(buffer, result.c_str(), std::min(static_cast<size_t>(1023), result.size()));
  return buffer;
}

char const* XT_CALL
XtPrintErrorInfoToString(XtErrorInfo const* info) 
{
  std::ostringstream stream;
  static thread_local char buffer[1024];
  std::memset(buffer, 0, sizeof(buffer));
  stream << XtPrintSystemToString(info->system);
  stream << " " << XtPrintCauseToString(info->service.cause);
  stream << " Error: " << info->fault << " (" << info->service.text << ")";
  auto result = stream.str();
  std::memcpy(buffer, result.c_str(), std::min(static_cast<size_t>(1023), result.size()));
  return buffer;
}