#include <xt/audio/XtAudio.h>
#include <xt/Private.hpp>
#include <xt/private/Platform.hpp>
#include <xt/private/Services.hpp>

#include <memory>
#include <thread>
#include <cstring>
#include <cassert>

XtVersion XT_CALL
XtAudioGetVersion(void) 
{ return { 1, 7 }; }

XtSystem XT_CALL 
XtAudioSetupToSystem(XtSetup setup)
{
  XT_ASSERT(XtSetupProAudio <= setup && setup <= XtSetupConsumerAudio);
  return XtPlatform::SetupToSystem(setup);
}

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
  XtAttributes result;
  XT_ASSERT(XtSampleUInt8 <= sample && sample <= XtSampleFloat32);
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

XtPlatform* XT_CALL
XtAudioInit(char const* id, void* window, XtOnError onError)
{
  XT_ASSERT(XtPlatform::instance == nullptr);
  auto result = std::make_unique<XtPlatform>();
  result->onError = onError;
  result->threadId = std::this_thread::get_id();
  result->id = id == nullptr || strlen(id) == 0? "XT-Audio": id;
  auto alsa = XtiCreateAlsaService();
  if(alsa) result->services.emplace_back(std::move(alsa));
  auto jack = XtiCreateJackService();
  if(jack) result->services.emplace_back(std::move(jack));
  auto asio = XtiCreateAsioService();
  if(asio) result->services.emplace_back(std::move(asio));
  auto pulse = XtiCreatePulseService();
  if(pulse) result->services.emplace_back(std::move(pulse));
  auto dsound = XtiCreateDSoundService();
  if(dsound) result->services.emplace_back(std::move(dsound));
  auto wasapi = XtiCreateWasapiService();
  if(wasapi) result->services.emplace_back(std::move(wasapi));
  return XtPlatform::instance = result.release();
}