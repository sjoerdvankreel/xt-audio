#include "xt-cpp.hpp"
#include "xt-audio.h"

/* Copyright (C) 2015-2016 Sjoerd van Kreel.
 *
 * This file is part of XT-Audio.
 *
 * XT-Audio is free software: you can redistribute it and/or modify it under the 
 * terms of the GNU Lesser General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later version.
 *
 * XT-Audio is distributed in the hope that it will be useful, but WITHOUT ANY 
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with XT-Audio. If not, see<http://www.gnu.org/licenses/>.
 */

namespace Xt {

// ---- local ----

static TraceCallback trace = nullptr;
static FatalCallback fatal = nullptr;

static void HandleError(XtError e) {
  if(e != 0)
    throw Exception(e); 
}

static void XT_CALL ForwardFatalCallback() {
  if(fatal)
    fatal();
  throw std::logic_error("Fatal error.");
}

static std::string WrapAndFreeCString(char* cString) {
  std::string result(cString == nullptr? "": cString);
  XtAudioFree(cString);
  return result;
}

static void XT_CALL 
ForwardTraceCallback(XtLevel level, const char* message) {
  if(trace)
    trace(static_cast<Xt::Level>(level), message);
}

struct StreamCallbackForwarder {
  static void XT_CALL Forward(
    const XtStream* stream, const void* input, void* output, int32_t frames,
    double time, uint64_t position, XtBool timeValid, XtError error, void* user) {

    auto s = static_cast<Stream*>(user);
    s->callback(*s, input, output, frames, time, position, timeValid != XtFalse, error, s->user);
  }
};

// ---- error ----

Exception::Exception(uint64_t e):
e(e) {}

uint64_t Exception::GetError() const {
  return e;
}

const char* Exception::what() const noexcept {
  return XtErrorGetText(GetError()); 
}

uint32_t Exception::GetFault(uint64_t error) { 
  return XtErrorGetFault(error); 
}

std::string Exception::GetText(uint64_t error) {
  return XtErrorGetText(error); 
}

Cause Exception::GetCause(uint64_t error) {
  return static_cast<Cause>(XtErrorGetCause(error));
}

System Exception::GetSystem(uint64_t error) { 
  return static_cast<System>(XtErrorGetSystem(error));
}

// ---- ostream ----

std::ostream& operator<<(std::ostream& os, Level level) {
  return os << Print::LevelToString(level);
}

std::ostream& operator<<(std::ostream& os, Cause cause) {
  return os << Print::CauseToString(cause);
}

std::ostream& operator<<(std::ostream& os, Setup setup) {
  return os << Print::SetupToString(setup);
}

std::ostream& operator<<(std::ostream& os, System system) {
  return os << Print::SystemToString(system);
}

std::ostream& operator<<(std::ostream& os, Sample sample) {
  return os << Print::SampleToString(sample);
}

std::ostream& operator<<(std::ostream& os, const Mix& mix) {
  return os << Print::MixToString(mix);
}

std::ostream& operator<<(std::ostream& os, const Buffer& buffer) {
  return os << Print::BufferToString(buffer);
}

std::ostream& operator<<(std::ostream& os, const Format& format) {
  return os << Print::FormatToString(format);
}

std::ostream& operator<<(std::ostream& os, const Device& device) {
  return os << device.GetName();
}

std::ostream& operator<<(std::ostream& os, const Service& service) {
  return os << service.GetName();
}

std::ostream& operator<<(std::ostream& os, const Latency& latency) {
  return os << Print::LatencyToString(latency);
}

std::ostream& operator<<(std::ostream& os, Capabilities capabilities) {
  return os << Print::CapabilitiesToString(capabilities);
}

std::ostream& operator<<(std::ostream& os, const Exception& exception) {
  return os << Print::ErrorToString(exception.GetError());
}

std::ostream& operator<<(std::ostream& os, const Attributes& attributes) {
  return os << Print::AttributesToString(attributes);
}

// ---- print ----

std::string Print::ErrorToString(uint64_t error) {
  return WrapAndFreeCString(XtPrintErrorToString(error));
}

std::string Print::LevelToString(Level level) {
  return XtPrintLevelToString(static_cast<XtLevel>(level));
}

std::string Print::CauseToString(Cause cause) {
  return XtPrintCauseToString(static_cast<XtCause>(cause));
}

std::string Print::SetupToString(Setup setup) {
  return XtPrintSetupToString(static_cast<XtSetup>(setup));
}

std::string Print::SystemToString(System system) {
  return XtPrintSystemToString(static_cast<XtSystem>(system));
}

std::string Print::SampleToString(Sample sample) {
  return XtPrintSampleToString(static_cast<XtSample>(sample));
}

std::string Print::MixToString(const Mix& mix) {
  return WrapAndFreeCString(XtPrintMixToString(reinterpret_cast<const XtMix*>(&mix)));
}

std::string Print::FormatToString(const Format& format) {
  return WrapAndFreeCString(XtPrintFormatToString(reinterpret_cast<const XtFormat*>(&format)));
}

std::string Print::BufferToString(const Buffer& buffer) {
  return WrapAndFreeCString(XtPrintBufferToString(reinterpret_cast<const XtBuffer*>(&buffer)));
}

std::string Print::LatencyToString(const Latency& latency) {
  return WrapAndFreeCString(XtPrintLatencyToString(reinterpret_cast<const XtLatency*>(&latency)));
}

std::string Print::CapabilitiesToString(Capabilities capabilities) {
  return WrapAndFreeCString(XtPrintCapabilitiesToString(static_cast<const XtCapabilities>(capabilities)));
}

std::string Print::AttributesToString(const Attributes& attributes) {
  return WrapAndFreeCString(XtPrintAttributesToString(reinterpret_cast<const XtAttributes*>(&attributes)));
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

std::string Audio::GetVersion() {
  return XtAudioGetVersion();
}

int32_t Audio::GetServiceCount() {
  return XtAudioGetServiceCount();
}

bool Audio::IsWin32() {
  return XtAudioIsWin32() != XtFalse;
}

Attributes Audio::GetSampleAttributes(Sample sample) {
  Attributes result;
  XtAttributes attributes;
  XtAudioGetSampleAttributes(static_cast<XtSample>(sample), &attributes);
  result.size = attributes.size;
  result.isFloat = attributes.isFloat != XtFalse;
  result.isSigned = attributes.isSigned != XtFalse;
  return result;
}

std::unique_ptr<Service> Audio::GetServiceByIndex(int32_t index) {
  return std::unique_ptr<Service>(new Service(XtAudioGetServiceByIndex(index)));
}

std::unique_ptr<Service> Audio::GetServiceBySetup(Setup setup) {
  return std::unique_ptr<Service>(new Service(XtAudioGetServiceBySetup(static_cast<XtSetup>(setup))));
}

std::unique_ptr<Service> Audio::GetServiceBySystem(System system) {
  return std::unique_ptr<Service>(new Service(XtAudioGetServiceBySystem(static_cast<XtSystem>(system))));
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
  char* name; 
  HandleError(XtDeviceGetName(d, &name)); 
  return WrapAndFreeCString(name);
}

void Device::ShowControlPanel() { 
  HandleError(XtDeviceShowControlPanel(d));
}

int32_t Device::GetChannelCount(bool output) const { 
  int32_t count; 
  HandleError(XtDeviceGetChannelCount(d, output, &count)); 
  return count; 
}

std::unique_ptr<Mix> Device::GetMix() const {
  XtMix* mix;
  HandleError(XtDeviceGetMix(d, &mix));
  if(mix == nullptr)
    return std::unique_ptr<Mix>();
  std::unique_ptr<Mix> result(new Mix(*reinterpret_cast<Mix*>(mix)));
  XtAudioFree(mix);
  return result;
}

bool Device::SupportsAccess(bool interleaved) const {
  XtBool supports;
  HandleError(XtDeviceSupportsAccess(d, interleaved, &supports));
  return supports != XtFalse;    
}

std::string Device::GetChannelName(bool output, int32_t index) const {
  char* name;
  HandleError(XtDeviceGetChannelName(d, output, index, &name)); 
  return WrapAndFreeCString(name);
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

std::unique_ptr<Stream> Device::OpenStream(const Format& format, bool interleaved, 
                                           double bufferSize, StreamCallback callback, void* user) {
  XtStream* stream; 
  auto f = reinterpret_cast<const XtFormat*>(&format);
  std::unique_ptr<Stream> result(new Stream(callback, user));
  HandleError(XtDeviceOpenStream(d, f, interleaved, bufferSize, &StreamCallbackForwarder::Forward, result.get(), &stream));
  result->s = stream;
  return result;
}

// ---- stream ----

Stream::Stream(StreamCallback callback, void* user):
s(nullptr), callback(callback), user(user) {}

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
