#include "xt-cpp.hpp"
#include "xt-audio.h"
#include <numeric>

namespace Xt {

// ---- local ----

static TraceCallback trace = nullptr;
static FatalCallback fatal = nullptr;

static void HandleError(XtError e) {
  if(e != 0)
    throw Exception(e); 
}

static void XT_CALLBACK ForwardFatalCallback() {
  if(fatal)
    fatal();
  throw std::logic_error("Fatal error.");
}

static void XT_CALLBACK
ForwardTraceCallback(XtLevel level, const char* message) {
  if(trace)
    trace(static_cast<Xt::Level>(level), message);
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

Exception::Exception(uint64_t e):
e(e) {}

uint64_t Exception::GetError() const {
  return e;
}

const char* Exception::what() const noexcept {
  return XtAudioGetErrorText(GetError()); 
}

// ---- ostream ----

std::ostream& operator<<(std::ostream& os, Level level) {
  return os << XtAudioPrintLevelToString(static_cast<XtLevel>(level));
}

std::ostream& operator<<(std::ostream& os, Cause cause) {
  return os << XtAudioPrintCauseToString(static_cast<XtCause>(cause));
}

std::ostream& operator<<(std::ostream& os, Setup setup) {
  return os << XtAudioPrintSetupToString(static_cast<XtSetup>(setup));
}

std::ostream& operator<<(std::ostream& os, System system) {
  return os << XtAudioPrintSystemToString(static_cast<XtSystem>(system));
}

std::ostream& operator<<(std::ostream& os, Sample sample) {
  return os << XtAudioPrintSampleToString(static_cast<XtSample>(sample));
}

std::ostream& operator<<(std::ostream& os, Capabilities capabilities) {
  auto strings = Audio::PrintCapabilitiesToString(capabilities);
  auto joiner = [](const std::string& l, const std::string& r) { return l.size() > 0? l + ", " + r: r; };
  return os << std::accumulate(strings.cbegin(), strings.cend(), std::string(), joiner);  
}

// ---- audio ----

Audio::~Audio() { 
  XtAudioTerminate(); 
  trace = nullptr; 
  fatal = nullptr;
}

Audio::Audio(const std::string& id, void* window, TraceCallback t, FatalCallback f) {
  fatal = f;
  trace = t;
  XtAudioInit(id.c_str(), window, &ForwardTraceCallback, &ForwardFatalCallback);
}

Version Audio::GetVersion() {
  auto result = XtAudioGetVersion();
  return *reinterpret_cast<Version*>(&result);
}

int32_t Audio::GetServiceCount() {
  return XtAudioGetServiceCount();
}

bool Audio::IsWin32() {
  return XtAudioIsWin32() != XtFalse;
}

uint32_t Audio::GetErrorFault(uint64_t error) { 
  return XtAudioGetErrorFault(error); 
}

std::string Audio::GetErrorText(uint64_t error) {
  return XtAudioGetErrorText(error); 
}

Cause Audio::GetErrorCause(uint64_t error) {
  return static_cast<Cause>(XtAudioGetErrorCause(error));
}

System Audio::GetErrorSystem(uint64_t error) { 
  return static_cast<System>(XtAudioGetErrorSystem(error));
}

Attributes Audio::GetSampleAttributes(Sample sample) {
  Attributes result;
  auto attrs = XtAudioGetSampleAttributes(static_cast<XtSample>(sample));
  result.size = attrs.size;
  result.isFloat = attrs.isFloat != XtFalse;
  result.isSigned = attrs.isSigned != XtFalse;
  return result;
}

std::unique_ptr<Service> Audio::GetServiceByIndex(int32_t index) {
  return std::unique_ptr<Service>(new Service(XtAudioGetServiceByIndex(index)));
}

std::unique_ptr<Service> Audio::GetServiceBySetup(Setup setup) {
  const XtService* service = XtAudioGetServiceBySetup(static_cast<XtSetup>(setup));
  return service? std::unique_ptr<Service>(new Service(service)): std::unique_ptr<Service>();
}

std::unique_ptr<Service> Audio::GetServiceBySystem(System system) {
  const XtService* service = XtAudioGetServiceBySystem(static_cast<XtSystem>(system));
  return service? std::unique_ptr<Service>(new Service(service)): std::unique_ptr<Service>();
}

std::vector<std::string> Audio::PrintCapabilitiesToString(Capabilities capabilities) {
  size_t i = 0;
  std::vector<std::string> result;
  const char* const* strings = XtAudioPrintCapabilitiesToString(static_cast<XtCapabilities>(capabilities));
  while(strings[i] != nullptr)
    result.emplace_back(std::string(strings[i++]));
  return result;
}

// ---- service ----

Service::Service(const XtService* s_):
s(s_) {}

std::string Service::GetName() const {
  return XtServiceGetName(s);
}

System Service::GetSystem() const {
  return static_cast<System>(XtServiceGetSystem(s));
}

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

System Device::GetSystem() const { 
  return static_cast<System>(XtDeviceGetSystem(d));
}

std::string Device::GetName() const { 
  int32_t size;
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
  int32_t size;
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

System Stream::GetSystem() const {
  return static_cast<System>(XtStreamGetSystem(s)); 
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