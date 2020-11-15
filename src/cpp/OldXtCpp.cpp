#include "XtCpp.hpp"
#include "XtAudio.h"
#include <numeric>

namespace Xt {

// ---- local ----



// ---- ostream ----

// ---- audio ----

Version Audio::GetVersion() {
  auto result = XtAudioGetVersion();
  return *reinterpret_cast<Version*>(&result);
}

ErrorInfo Audio::GetErrorInfo(uint64_t error) { 
  ErrorInfo result;
  auto info = XtAudioGetErrorInfo(error);
  result.fault = info.fault;
  result.text = std::string(info.text);
  result.cause = static_cast<Cause>(info.cause);
  result.system = static_cast<System>(info.system);
  return result;
}

Attributes Audio::GetSampleAttributes(Sample sample) {
  Attributes result;
  auto attrs = XtAudioGetSampleAttributes(static_cast<XtSample>(sample));
  result.size = attrs.size;
  result.count = attrs.count;
  result.isFloat = attrs.isFloat != XtFalse;
  result.isSigned = attrs.isSigned != XtFalse;
  return result;
}

std::vector<System> Audio::GetSystems() {
  int32_t size = 0;
  XtAudioGetSystems(nullptr, &size);
  std::vector<System> result(static_cast<size_t>(size));
  XtAudioGetSystems(reinterpret_cast<XtSystem*>(result.data()), &size);
  return result;
}

System Audio::SetupToSystem(Setup setup) {
  return static_cast<System>(XtAudioSetupToSystem(static_cast<XtSetup>(setup)));
}

std::unique_ptr<Service> Audio::GetService(System system) {
  const XtService* service = XtAudioGetService(static_cast<XtSystem>(system));
  return service? std::unique_ptr<Service>(new Service(service)): std::unique_ptr<Service>();
}

std::unique_ptr<void, void(*)(void*)> Audio::Init(const std::string& id, void* window, OnError onError) {
  _onError = onError;
  static int32_t token = 0;
  XtAudioInit(id.c_str(), window, &ForwardOnError);
  return std::unique_ptr<void, void(*)(void*)>(static_cast<void*>(&token), [](void*) { XtAudioTerminate(); });
}


// ---- service ----

Service::Service(const XtService* s_):
s(s_) {}

int32_t Service::GetDeviceCount() const { 
  int32_t count; 
  HandleError(XtServiceGetDeviceCount(s, &count));
  return count;
}

Capabilities Service::GetCapabilities() const {
  return static_cast<Capabilities>(XtServiceGetCapabilities(s));
}

std::unique_ptr<Device> Service::OpenDevice(int32_t index) const { 
  XtDevice* device; 
  HandleError(XtServiceOpenDevice(s, index, &device));
  return std::unique_ptr<Device>(new Device(device));
}

std::unique_ptr<Device> Service::OpenDefaultDevice(bool output) const{ 
  XtDevice* device; 
  HandleError(XtServiceOpenDefaultDevice(s, output != XtFalse, &device));
  if(device == nullptr)
    return std::unique_ptr<Device>();
  return std::unique_ptr<Device>(new Device(device));
}

std::unique_ptr<Stream> Service::AggregateStream(const AggregateStreamParams& params, void* user) {
  XtStream* stream; 
  std::vector<XtAggregateDeviceParams> ds(params.count);
  for(int32_t i = 0; i < params.count; i++)
  {
    ds[i].device = params.devices[i].device->d;
    ds[i].bufferSize = params.devices[i].bufferSize;
    ds[i].channels = *reinterpret_cast<const XtChannels*>(&params.devices[i].channels);
  }
  XtAggregateStreamParams coreParams;
  std::memset(&coreParams, 0, sizeof(coreParams));
  coreParams.devices = ds.data();
  coreParams.count = params.count;
  coreParams.master = params.master->d;
  coreParams.stream.interleaved = params.stream.interleaved;
  coreParams.mix = *reinterpret_cast<const XtMix*>(&params.mix);
  coreParams.stream.onBuffer = &StreamCallbackForwarder::ForwardOnBuffer;
  coreParams.stream.onXRun = params.stream.onXRun == nullptr? nullptr: &StreamCallbackForwarder::ForwardOnXRun;
  std::unique_ptr<Stream> result(new Stream(params.stream.onBuffer, params.stream.onXRun, user));
  HandleError(XtServiceAggregateStream(s, &coreParams, result.get(), &stream));
  result->s = stream;
  return result;
}

// ---- device ----

// ---- stream ----

} // namespace Xt