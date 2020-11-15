#include "XtCpp.hpp"
#include "XtAudio.h"
#include <numeric>

namespace Xt {

// ---- local ----

static OnError _onError = nullptr;

static void HandleError(XtError error) {
  if(error != 0) throw Exception(error);
}

static void XT_CALLBACK
ForwardOnError(const char* location, const char* message) {
  if(_onError) _onError(location, message);
}

struct StreamCallbackForwarder {
  static void XT_CALLBACK ForwardOnXRun(int32_t index, void* user) {

    auto s = static_cast<Stream*>(user);
    s->onXRun(index, s->user);
  }

  static void XT_CALLBACK ForwardStream(
    const XtStream* stream, const XtBuffer* buffer, void* user) {

    Buffer b;
    auto s = static_cast<Stream*>(user);
    b.position = buffer->position;
    b.time = buffer->time;
    b.timeValid = buffer->timeValid != XtFalse;
    b.frames = buffer->frames;
    b.input = buffer->input; 
    b.output = buffer->output; 
    b.error = buffer->error;
    s->streamCallback(*s, b, s->user);
  }
};

// ---- ostream ----

std::ostream& operator<<(std::ostream& os, const Device& device) {
  return os << device.GetName();
}

std::ostream& operator<<(std::ostream& os, const ErrorInfo& info) {
  XtErrorInfo i;
  i.fault = info.fault;
  i.text = info.text.c_str();
  i.cause = static_cast<XtCause>(info.cause);
  i.system = static_cast<XtSystem>(info.system);
  return os << XtPrintErrorInfoToString(&i);
}

std::ostream& operator<<(std::ostream& os, Cause cause) {
  return os << XtPrintCauseToString(static_cast<XtCause>(cause));
}

std::ostream& operator<<(std::ostream& os, Setup setup) {
  return os << XtPrintSetupToString(static_cast<XtSetup>(setup));
}

std::ostream& operator<<(std::ostream& os, System system) {
  return os << XtPrintSystemToString(static_cast<XtSystem>(system));
}

std::ostream& operator<<(std::ostream& os, Sample sample) {
  return os << XtPrintSampleToString(static_cast<XtSample>(sample));
}

std::ostream& operator<<(std::ostream& os, Capabilities capabilities) {
  return os << XtPrintCapabilitiesToString(static_cast<XtCapabilities>(capabilities));
}

// ---- audio ----

Audio::~Audio() { 
  XtAudioTerminate(); 
  _onError = nullptr; 
}

Audio::Audio(const std::string& id, void* window, OnError onError) {
  _onError = onError;
  XtAudioInit(id.c_str(), window, &ForwardOnError);
}

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
  coreParams.stream.streamCallback = &StreamCallbackForwarder::ForwardStream;
  coreParams.stream.onXRun = params.stream.onXRun == nullptr? nullptr: &StreamCallbackForwarder::ForwardOnXRun;
  std::unique_ptr<Stream> result(new Stream(params.stream.streamCallback, params.stream.onXRun, user));
  HandleError(XtServiceAggregateStream(s, &coreParams, result.get(), &stream));
  result->s = stream;
  return result;
}

// ---- device ----

Device::~Device() { 
  XtDeviceDestroy(d); 
}

Device::Device(XtDevice* d): 
d(d) {}

std::string Device::GetName() const { 
  int32_t size = 0;
  HandleError(XtDeviceGetName(d, nullptr, &size));
  std::vector<char> buffer(static_cast<size_t>(size));
  HandleError(XtDeviceGetName(d, buffer.data(), &size));
  return std::string(buffer.data());
}

void Device::ShowControlPanel() { 
  HandleError(XtDeviceShowControlPanel(d));
}

int32_t Device::GetChannelCount(bool output) const { 
  int32_t count; 
  HandleError(XtDeviceGetChannelCount(d, output, &count)); 
  return count; 
}

std::optional<Mix> Device::GetMix() const {
  Mix mix;
  XtBool valid;
  HandleError(XtDeviceGetMix(d, &valid, reinterpret_cast<XtMix*>(&mix)));
  return valid? std::optional<Mix>(mix): std::optional<Mix>(std::nullopt);
}

bool Device::SupportsAccess(bool interleaved) const {
  XtBool supports;
  HandleError(XtDeviceSupportsAccess(d, interleaved, &supports));
  return supports != XtFalse;    
}

std::string Device::GetChannelName(bool output, int32_t index) const {
  int32_t size = 0;
  HandleError(XtDeviceGetChannelName(d, output, index, nullptr, &size));
  std::vector<char> buffer(static_cast<size_t>(size));
  HandleError(XtDeviceGetChannelName(d, output, index, buffer.data(), &size));
  return std::string(buffer.data());
}

bool Device::SupportsFormat(const Format& f) const {
  XtBool supports; 
  HandleError(XtDeviceSupportsFormat(d, reinterpret_cast<const XtFormat*>(&f), &supports)); 
  return supports != XtFalse; 
}

BufferSize Device::GetBufferSize(const Format& format) const { 
  BufferSize result; 
  HandleError(XtDeviceGetBufferSize(d, reinterpret_cast<const XtFormat*>(&format), reinterpret_cast<XtBufferSize*>(&result)));
  return result;
}

std::unique_ptr<Stream> Device::OpenStream(const DeviceStreamParams& params, void* user) {
  XtStream* stream; 
  XtDeviceStreamParams coreParams = { 0 };
  coreParams.bufferSize = params.bufferSize;
  coreParams.stream.interleaved = params.stream.interleaved;
  coreParams.stream.streamCallback = &StreamCallbackForwarder::ForwardStream;
  coreParams.format = *reinterpret_cast<const XtFormat*>(&params.format);
  coreParams.stream.onXRun = params.stream.onXRun == nullptr? nullptr: &StreamCallbackForwarder::ForwardOnXRun;
  std::unique_ptr<Stream> result(new Stream(params.stream.streamCallback, params.stream.onXRun, user));
  HandleError(XtDeviceOpenStream(d, &coreParams, result.get(), &stream));
  result->s = stream;
  return result;
}

// ---- stream ----

Stream::Stream(StreamCallback streamCallback, OnXRun onXRun, void* user):
s(nullptr),
onXRun(onXRun),
streamCallback(streamCallback), 
user(user) {}

Stream::~Stream() { 
  XtStreamDestroy(s);
}

void Stream::Stop() {
  HandleError(XtStreamStop(s)); 
}

void Stream::Start() { 
  HandleError(XtStreamStart(s));
}

int32_t Stream::GetFrames() const {
  int32_t frames;
  HandleError(XtStreamGetFrames(s, &frames));
  return frames;
}

const Format& Stream::GetFormat() const {
  return *reinterpret_cast<const Format*>(XtStreamGetFormat(s));
}

Latency Stream::GetLatency() const {
  Latency latency;
  HandleError(XtStreamGetLatency(s, reinterpret_cast<XtLatency*>(&latency)));
  return latency;
}

} // namespace Xt