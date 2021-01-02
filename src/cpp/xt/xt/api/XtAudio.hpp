#ifndef XT_API_AUDIO_HPP
#define XT_API_AUDIO_HPP

/** @file */

#include <xt/api/Core.hpp>
#include <xt/api/Structs.hpp>
#include <xt/api/XtService.hpp>
#include <xt/api/XtPlatform.hpp>

#include <memory>
#include <vector>
#include <string>

namespace Xt {

using namespace Detail;

class Audio final 
{
  Audio() = default;
public:
  static Version GetVersion();
  static ErrorInfo GetErrorInfo(uint64_t error);
  static Attributes GetSampleAttributes(Sample sample);
  static std::unique_ptr<Platform> Init(std::string const& id, void* window, OnError onError);
};

inline Version
Audio::GetVersion() 
{
  auto result = XtAudioGetVersion();
  return *reinterpret_cast<Version*>(&result);
}

inline Attributes 
Audio::GetSampleAttributes(Sample sample) 
{
  Attributes result;
  auto coreSample = static_cast<XtSample>(sample);
  auto attrs = XtAudioGetSampleAttributes(coreSample);
  result.size = attrs.size;
  result.count = attrs.count;
  result.isFloat = attrs.isFloat != XtFalse;
  result.isSigned = attrs.isSigned != XtFalse;
  return result;
}

inline ErrorInfo
Audio::GetErrorInfo(uint64_t error) 
{ 
  ErrorInfo result;
  auto info = XtAudioGetErrorInfo(error);
  result.fault = info.fault;
  result.system = static_cast<System>(info.system);
  result.service.text = std::string(info.service.text);
  result.service.cause = static_cast<Cause>(info.service.cause);
  return result;
}

inline std::unique_ptr<Platform>
Audio::Init(std::string const& id, void* window, OnError onError) 
{
  Detail::_onError = onError;
  XtOnError coreOnError = onError == nullptr? nullptr: &Detail::ForwardOnError;
  XtPlatform* result = XtAudioInit(id.c_str(), window, coreOnError);
  return std::unique_ptr<Platform>(new Platform(result));
}

} // namespace Xt
#endif // XT_API_AUDIO_HPP