#include <xt/api/XtAudio.h>
#include <xt/shared/Services.hpp>
#include <xt/private/Platform.hpp>

#include <memory>
#include <thread>
#include <cstring>

XtVersion XT_CALL
XtAudioGetVersion(void) 
{ return { 2, 0 }; }
char const* XT_CALL
XtAudioGetLastAssert(void)
{ return XtiGetLastAssert(); }
void XT_CALL
XtAudioSetOnError(XtOnError onError)
{ XtiSetOnError(onError); }
void XT_CALL
XtAudioSetAssertTerminates(XtBool terminates)
{ XtiSetAssertTerminates(terminates); }

XtErrorInfo XT_CALL
XtAudioGetErrorInfo(XtError error) 
{
  XtErrorInfo result;
  XT_ASSERT_API(error != 0);
  auto fault = XtiGetErrorFault(error);
  auto sysid = (error & 0xFFFFFFFF00000000) >> 32ULL;
  auto system = static_cast<XtSystem>(sysid);
  result.fault = fault;
  result.system = system;
  result.service = XtiGetServiceError(system, fault);
  return result;
}

XtAttributes XT_CALL
XtAudioGetSampleAttributes(XtSample sample) 
{
  XtAttributes result;
  XT_ASSERT_API(XtSampleUInt8 <= sample && sample <= XtSampleFloat32);
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
  default: XT_ASSERT(false);
  }
  return result;
}

XtPlatform* XT_CALL
XtAudioInit(char const* id, void* window)
{
  XT_ASSERT_API(XtPlatform::instance == nullptr);
  auto result = std::make_unique<XtPlatform>();
  if(!result->Init(window)) return nullptr;
  result->_threadId = std::this_thread::get_id();
  result->_id = id == nullptr || strlen(id) == 0? "XT-Audio": id;
  auto alsa = XtiCreateAlsaService();
  if(alsa) result->_services.emplace_back(std::move(alsa));
  auto jack = XtiCreateJackService();
  if(jack) result->_services.emplace_back(std::move(jack));
  auto asio = XtiCreateAsioService();
  if(asio) result->_services.emplace_back(std::move(asio));
  auto pulse = XtiCreatePulseService();
  if(pulse) result->_services.emplace_back(std::move(pulse));
  auto dsound = XtiCreateDSoundService();
  if(dsound) result->_services.emplace_back(std::move(dsound));
  auto wasapi = XtiCreateWasapiService();
  if(wasapi) result->_services.emplace_back(std::move(wasapi));
  return XtPlatform::instance = result.release();
}