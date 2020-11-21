#include <xt/audio/XtAudio.h>
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
  auto service = XtAudioGetService(system);
  result.fault = fault;
  result.system = system;
  result.text = service->GetFaultText(fault);
  result.cause = service->GetFaultCause(fault);
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
  XT_ASSERT(XtSystemALSA <= system && system <= XtSystemDirectSound);
  switch(system) 
  {
  case XtSystemALSA: return XtiGetAlsaService();
  case XtSystemASIO: return XtiGetAsioService();
  case XtSystemJACK: return XtiGetJackService();
  case XtSystemWASAPI: return XtiGetWasapiService();
  case XtSystemPulseAudio: return XtiGetPulseAudioService();
  case XtSystemDirectSound: return XtiGetDirectSoundService();
  default: assert(false); return nullptr;
  }
}

void XT_CALL 
XtAudioGetSystems(XtSystem* buffer, int32_t* size) 
{
  std::vector<XtSystem> systems;
  if(XtiGetAsioService() != nullptr) systems.push_back(XtiGetAsioService()->GetSystem());
  if(XtiGetJackService() != nullptr) systems.push_back(XtiGetJackService()->GetSystem());
  if(XtiGetAlsaService() != nullptr) systems.push_back(XtiGetAlsaService()->GetSystem());
  if(XtiGetWasapiService() != nullptr) systems.push_back(XtiGetWasapiService()->GetSystem());
  if(XtiGetPulseAudioService() != nullptr) systems.push_back(XtiGetPulseAudioService()->GetSystem());
  if(XtiGetDirectSoundService() != nullptr) systems.push_back(XtiGetDirectSoundService()->GetSystem());
  auto count = static_cast<int32_t>(systems.size());
  if(buffer == nullptr) *size = count;
  else memcpy(buffer, systems.data(), std::min(*size, count)*sizeof(XtSystem));
}

XtSystem XT_CALL 
XtAudioSetupToSystem(XtSetup setup) 
{
  XT_ASSERT(XtSetupProAudio <= setup && setup <= XtSetupConsumerAudio);
  switch(setup) 
  {
  case XtSetupProAudio: return XtiGetAsioService()? XtSystemASIO: XtSystemJACK;
  case XtSetupSystemAudio: return XtiGetWasapiService()? XtSystemWASAPI: XtSystemALSA;
  case XtSetupConsumerAudio: return XtiGetDirectSoundService()? XtSystemDirectSound: XtSystemPulseAudio;
  default: assert(false); return static_cast<XtSystem>(0);
  }
}