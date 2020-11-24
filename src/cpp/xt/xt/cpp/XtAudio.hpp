#ifndef XT_CPP_AUDIO_HPP
#define XT_CPP_AUDIO_HPP

/** @file */

#include <xt/cpp/Core.hpp>
#include <xt/cpp/Structs.hpp>
#include <xt/cpp/XtService.hpp>
#include <xt/cpp/XtPlatform.hpp>

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
  static System SetupToSystem(Setup setup);
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

inline System
Audio::SetupToSystem(Setup setup)
{ 
  auto coreSetup = static_cast<XtSetup>(setup);
  auto result = XtAudioSetupToSystem(coreSetup);
  return static_cast<System>(result); 
}

inline ErrorInfo
Audio::GetErrorInfo(uint64_t error) 
{ 
  ErrorInfo result;
  auto info = XtAudioGetErrorInfo(error);
  result.fault = info.fault;
  result.text = std::string(info.text);
  result.cause = static_cast<Cause>(info.cause);
  result.system = static_cast<System>(info.system);
  return result;
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

inline std::unique_ptr<Platform>
Audio::Init(std::string const& id, void* window, OnError onError) 
{
  Detail::_onError = onError;
  XtOnError coreOnError = onError == nullptr? nullptr: &Detail::ForwardOnError;
  XtPlatform* result = XtAudioInit(id.c_str(), window, coreOnError);
  return std::unique_ptr<Platform>(new Platform(result));
}

} // namespace Xt
#endif // XT_CPP_AUDIO_HPP