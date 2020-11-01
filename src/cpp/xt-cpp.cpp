#include "xt-cpp.hpp"
#include "xt-audio.h"
#include <numeric>

namespace Xt {

// ---- local ----

static ErrorCallback _errorCallback = nullptr;

static void HandleError(XtError error) {
  if(error != 0) throw Exception(error);
}

static void XT_CALLBACK
ForwardErrorCallback(const char* location, const char* message) {
  if(_errorCallback) _errorCallback(location, message);
}

struct StreamCallbackForwarder {
  static void XT_CALLBACK ForwardXRun(int32_t index, void* user) {

    auto s = static_cast<Stream*>(user);
    s->xRunCallback(index, s->user);
  }

  static void XT_CALLBACK ForwardStream(
    const XtStream* stream, const void* input, void* output, int32_t frames,
    double time, uint64_t position, XtBool timeValid, XtError error, void* user) {

    auto s = static_cast<Stream*>(user);
    s->streamCallback(*s, input, output, frames, time, position, timeValid != XtFalse, error, s->user);
  }
};

// ---- error ----

ErrorInfo Exception::GetInfo() const { 
  ErrorInfo result;
  auto info = XtAudioGetErrorInfo(_error);
  result.fault = info.fault;
  result.text = std::string(info.text);
  result.cause = static_cast<Cause>(info.cause);
  result.system = static_cast<System>(info.system);
  return result;
}

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
  _errorCallback = nullptr; 
}

Audio::Audio(const std::string& id, void* window, ErrorCallback error) {
  _errorCallback = error;
  XtAudioInit(id.c_str(), window, &ForwardErrorCallback);
}

Version Audio::GetVersion() {
  auto result = XtAudioGetVersion();
  return *reinterpret_cast<Version*>(&result);
}

Attributes Audio::GetSampleAttributes(Sample sample) {
  Attributes result;
  auto attrs = XtAudioGetSampleAttributes(static_cast<XtSample>(sample));
  result.size = attrs.size;
  result.isFloat = attrs.isFloat != XtFalse;
  result.isSigned = attrs.isSigned != XtFalse;
  return result;
}

System Audio::SetupToSystem(Setup setup) {
  return static_cast<System>(XtAudioSetupToSystem(static_cast<XtSetup>(setup)));
}

std::vector<System> Audio::GetSystems() {
  int32_t size = 0;
  XtAudioGetSystems(nullptr, &size);
  std::vector<System> result(static_cast<size_t>(size));
  XtAudioGetSystems(reinterpret_cast<XtSystem*>(result.data()), &size);
  return result;
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

std::unique_ptr<Stream> Service::AggregateStream(Device** devices, const Channels* channels, 
                                                 const double* bufferSizes, int32_t count, 
                                                 const Mix& mix, bool interleaved, Device& master, 
                                                 StreamCallback streamCallback, XRunCallback xRunCallback, void* user) {


  XtStream* stream; 
  std::vector<XtDevice*> ds(count, nullptr);
  for(int32_t i = 0; i < count; i++)
    ds[i] = devices[i]->d;
  auto m = reinterpret_cast<const XtMix*>(&mix);
  auto c = reinterpret_cast<const XtChannels*>(channels);
  auto forwardStream = &StreamCallbackForwarder::ForwardStream;
  auto forwardXRun = xRunCallback == nullptr? nullptr: &StreamCallbackForwarder::ForwardXRun;
  std::unique_ptr<Stream> result(new Stream(streamCallback, xRunCallback, user));
  HandleError(XtServiceAggregateStream(s, &ds[0], c, bufferSizes, count, m, interleaved, master.d, forwardStream, forwardXRun, result.get(), &stream));
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

Buffer Device::GetBuffer(const Format& format) const { 
  Buffer buffer; 
  HandleError(XtDeviceGetBuffer(d, reinterpret_cast<const XtFormat*>(&format), reinterpret_cast<XtBuffer*>(&buffer)));
  return buffer;
}

std::unique_ptr<Stream> Device::OpenStream(const Format& format, bool interleaved, double bufferSize,
                                           StreamCallback streamCallback, XRunCallback xRunCallback, void* user) {

  XtStream* stream; 
  auto f = reinterpret_cast<const XtFormat*>(&format);
  auto forwardStream = &StreamCallbackForwarder::ForwardStream;
  auto forwardXRun = xRunCallback == nullptr? nullptr: &StreamCallbackForwarder::ForwardXRun;
  std::unique_ptr<Stream> result(new Stream(streamCallback, xRunCallback, user));
  HandleError(XtDeviceOpenStream(d, f, interleaved, bufferSize, forwardStream, forwardXRun, result.get(), &stream));
  result->s = stream;
  return result;
}

// ---- stream ----

Stream::Stream(StreamCallback streamCallback, XRunCallback xRunCallback, void* user):
s(nullptr),
xRunCallback(xRunCallback),
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

bool Stream::IsInterleaved() const { 
  return XtStreamIsInterleaved(s) != XtFalse;
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