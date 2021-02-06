#include <xt/api/XtPrint.h>
#include <xt/shared/Shared.hpp>

#include <string>
#include <cstring>
#include <sstream>
#include <algorithm>

char const* XT_CALL
XtPrintSample(XtSample sample) 
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
XtPrintCause(XtCause cause) 
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
XtPrintSystem(XtSystem system) 
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
XtPrintEnumFlags(XtEnumFlags flags)
{
  if(flags == XtEnumFlagsAll) return "All";
  if(flags == XtEnumFlagsInput) return "Input";
  if(flags == XtEnumFlagsOutput) return "Output";
  XT_ASSERT(false); return nullptr;
}

char const* XT_CALL
XtPrintSetup(XtSetup setup) 
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
XtPrintDeviceCaps(XtDeviceCaps capabilities) 
{
  size_t i = 0;
  std::string result;
  if(capabilities == 0) return "None";
  static thread_local char buffer[128];
  if((capabilities & XtDeviceCapsInput) != 0) result += "Input, ";
  if((capabilities & XtDeviceCapsOutput) != 0) result += "Output, ";
  if((capabilities & XtDeviceCapsLoopback) != 0) result += "Loopback, ";
  if((capabilities & XtDeviceCapsHwDirect) != 0) result += "HwDirect, ";
  std::memcpy(buffer, result.data(), result.size() - 2);
  buffer[result.size() - 2] = '\0';
  return buffer;
}

char const* XT_CALL
XtPrintServiceCaps(XtServiceCaps capabilities) 
{
  size_t i = 0;
  std::string result;
  if(capabilities == 0) return "None";
  static thread_local char buffer[256];
  if((capabilities & XtServiceCapsTime) != 0) result += "Time, ";
  if((capabilities & XtServiceCapsLatency) != 0) result += "Latency, ";
  if((capabilities & XtServiceCapsFullDuplex) != 0) result += "FullDuplex, ";
  if((capabilities & XtServiceCapsChannelMask) != 0) result += "ChannelMask, ";
  if((capabilities & XtServiceCapsAggregation) != 0) result += "Aggregation, ";
  if((capabilities & XtServiceCapsControlPanel) != 0) result += "ControlPanel, ";
  if((capabilities & XtServiceCapsXRunDetection) != 0) result += "XRunDetection, ";
  std::memcpy(buffer, result.data(), result.size() - 2);
  buffer[result.size() - 2] = '\0';
  return buffer;
}

char const* XT_CALL
XtPrintErrorInfo(XtErrorInfo const* info) 
{
  std::ostringstream stream;
  static thread_local char buffer[1024];
  std::memset(buffer, 0, sizeof(buffer));
  stream << XtPrintSystem(info->system);
  stream << " " << XtPrintCause(info->service.cause);
  stream << " Error: " << info->fault << " (" << info->service.text << ")";
  auto result = stream.str();
  std::memcpy(buffer, result.c_str(), std::min(static_cast<size_t>(1023), result.size()));
  return buffer;
}