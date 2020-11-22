#include <xt/audio/XtAudio.h>
#include <xt/private/Shared.hpp>
#include <xt/Private.hpp>
#include <cassert>
#include <algorithm>

XtVersion XT_CALL
XtAudioGetVersion(void) 
{ return { 1, 7 }; }

XtErrorInfo XT_CALL
XtAudioGetErrorInfo(XtError error) 
{
  XtErrorInfo result;
  XT_ASSERT(error != 0);
  auto fault = XtiGetErrorFault(error);
  auto sysid = (error & 0xFFFFFFFF00000000) >> 32ULL;
  auto system = static_cast<XtSystem>(sysid);
  result.fault = fault;
  result.system = system;
  result.text = XtiGetFaultText(system, fault);
  result.cause = XtiGetFaultCause(system, fault);
  return result;
}

XtAttributes XT_CALL
XtAudioGetSampleAttributes(XtSample sample) 
{
  XT_ASSERT(XtSampleUInt8 <= sample && sample <= XtSampleFloat32);
  XtAttributes result;
  result.isSigned = sample != XtSampleUInt8;
  result.isFloat = sample == XtSampleFloat32;
  result.count = sample == XtSampleInt24? 3: 1;
  switch(sample) 
  {
  case XtSampleUInt8: result.size = 1; break;
  case XtSampleInt16: result.size = 2; break;
  case XtSampleInt24: result.size = 3; break;
  case XtSampleInt32: result.size = 4; break;
  case XtSampleFloat32: result.size = 4; break;
  default: assert(false);
  }
  return result;
}

XtService const* XT_CALL 
XtAudioGetService(XtSystem system) 
{
  XT_ASSERT(XtSystemALSA <= system && system <= XtSystemDSound);
  switch(system) 
  {
  case XtSystemALSA: return XtiGetAlsaService();
  case XtSystemASIO: return XtiGetAsioService();
  case XtSystemJACK: return XtiGetJackService();
  case XtSystemPulse: return XtiGetPulseService();
  case XtSystemWASAPI: return XtiGetWasapiService();
  case XtSystemDSound: return XtiGetDSoundService();
  default: assert(false); return nullptr;
  }
}

XtSystem XT_CALL 
XtAudioSetupToSystem(XtSetup setup) 
{
  XT_ASSERT(XtSetupProAudio <= setup && setup <= XtSetupConsumerAudio);
  switch(setup) 
  {
  case XtSetupProAudio: return XtiGetAsioService()? XtSystemASIO: XtSystemJACK;
  case XtSetupSystemAudio: return XtiGetWasapiService()? XtSystemWASAPI: XtSystemALSA;
  case XtSetupConsumerAudio: return XtiGetDSoundService()? XtSystemDSound: XtSystemPulse;
  default: assert(false); return static_cast<XtSystem>(0);
  }
}

void XT_CALL 
XtAudioGetSystems(XtSystem* buffer, int32_t* size) 
{
  std::vector<XtSystem> systems;
  if(XtiGetAsioService() != nullptr) systems.push_back(XtiGetAsioService()->GetSystem());
  if(XtiGetJackService() != nullptr) systems.push_back(XtiGetJackService()->GetSystem());
  if(XtiGetAlsaService() != nullptr) systems.push_back(XtiGetAlsaService()->GetSystem());
  if(XtiGetPulseService() != nullptr) systems.push_back(XtiGetPulseService()->GetSystem());
  if(XtiGetWasapiService() != nullptr) systems.push_back(XtiGetWasapiService()->GetSystem());
  if(XtiGetDSoundService() != nullptr) systems.push_back(XtiGetDSoundService()->GetSystem());
  auto count = static_cast<int32_t>(systems.size());
  if(buffer == nullptr) *size = count;
  else memcpy(buffer, systems.data(), std::min(*size, count)*sizeof(XtSystem));
}