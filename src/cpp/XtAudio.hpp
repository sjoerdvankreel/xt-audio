#ifndef XT_AUDIO_HPP
#define XT_AUDIO_HPP

#include "XtService.hpp"
#include "CoreStructs.hpp"

#include <XtAudio.h>
#include <memory>
#include <vector>
#include <string>

namespace Xt {

class Audio final 
{
  Audio() = default;
  typedef std::unique_ptr<void, void(*)(void*)> Disposer;
public:
  static Version GetVersion();
  static std::vector<System> GetSystems();
  static System SetupToSystem(Setup setup); 
  static ErrorInfo GetErrorInfo(uint64_t error);
  static Attributes GetSampleAttributes(Sample sample);
  static std::unique_ptr<Service> GetService(System system);
  static Disposer Init(std::string const& id, void* window, OnError onError);
};

inline Version
Audio::GetVersion() 
{
  auto result = XtAudioGetVersion();
  return *reinterpret_cast<Version*>(&result);
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

inline System 
Audio::SetupToSystem(Setup setup) 
{
  auto coreSetup = static_cast<XtSetup>(setup);
  auto coreSystem = XtAudioSetupToSystem(coreSetup);
  return static_cast<System>(coreSystem);
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

inline std::unique_ptr<Service> 
Audio::GetService(System system) 
{
  auto coreSystem = static_cast<XtSystem>(system);
  XtService const* service = XtAudioGetService(coreSystem);
  if(!service) return std::unique_ptr<Service>();
  return std::unique_ptr<Service>(new Service(service));
}

inline std::vector<System> 
Audio::GetSystems() 
{
  int32_t size = 0;
  XtAudioGetSystems(nullptr, &size);
  std::vector<System> result(static_cast<size_t>(size));
  auto coreSystems = reinterpret_cast<XtSystem*>(result.data());
  XtAudioGetSystems(coreSystems, &size);
  return result;
}

inline Audio::Disposer
Audio::Init(std::string const& id, void* window, OnError onError) 
{
  static int32_t token = 0;
  Detail::_onError = onError;
  XtAudioInit(id.c_str(), window, &Detail::ForwardOnError);
  auto disposer = [](void*) { XtAudioTerminate(); };
  return Disposer(static_cast<void*>(&token), disposer);
}

} // namespace Xt
#endif // XT_AUDIO_HPP