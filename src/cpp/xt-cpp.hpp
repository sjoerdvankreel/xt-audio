#ifndef _XT_AUDIO_HPP
#define _XT_AUDIO_HPP

/** @file */

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

#include <memory>
#include <string>
#include <stdexcept>

struct XtStream;
struct XtDevice;
struct XtService;

namespace Xt {

class Stream;
class Device;
class Service;
class Exception;

enum class Level {
  Info,
  Error,
  Fatal
};

enum class Cause {
  Format,
  Service,
  Generic,
  Unknown,
  Endpoint
};

enum class System {
  Alsa = 1,
  Asio,
  Jack,
  Pulse,
  DSound,
  Wasapi
};

enum class Sample {
  UInt8,
  Int16,
  Int24,
  Int32,
  Float32
};

enum class Setup {
  ProAudio,
  SystemAudio,
  ConsumerAudio
};

enum Capabilities {
  CapabilitiesNone = 0x0,
  CapabilitiesTime = 0x1,
  CapabilitiesLatency = 0x2,
  CapabilitiesFullDuplex = 0x4,
  CapabilitiesChannelMask = 0x8
};

struct Buffer final {
  double min;
  double max;
  double current;
};

struct Latency final {
  double input;
  double output;
};

struct Attributes final {
  int32_t size;
  bool isFloat;
  bool isSigned;
};

struct Mix final {
  int32_t rate;
  Sample sample;

  Mix() = default;
  Mix(int32_t rate, Sample sample):
  rate(rate), sample(sample) {}
};

struct Format final {
  Mix mix;
  int32_t inputs;
  uint64_t inMask;
  int32_t outputs;
  uint64_t outMask;
  
  Format() = default;
  Format(const Mix& mix, int32_t inputs, 
    uint64_t inMask, int32_t outputs, uint64_t outMask):
  mix(mix), 
  inputs(inputs), inMask(inMask), 
  outputs(outputs), outMask(outMask){}
};

std::ostream& operator<<(std::ostream& os, Level level);
std::ostream& operator<<(std::ostream& os, Cause cause);
std::ostream& operator<<(std::ostream& os, Setup setup);
std::ostream& operator<<(std::ostream& os, System system);
std::ostream& operator<<(std::ostream& os, Sample sample);
std::ostream& operator<<(std::ostream& os, const Mix& mix);
std::ostream& operator<<(std::ostream& os, const Buffer& buffer);
std::ostream& operator<<(std::ostream& os, const Format& format);
std::ostream& operator<<(std::ostream& os, const Device& device);
std::ostream& operator<<(std::ostream& os, const Service& service);
std::ostream& operator<<(std::ostream& os, const Latency& latency);
std::ostream& operator<<(std::ostream& os, Capabilities capabilities);
std::ostream& operator<<(std::ostream& os, const Exception& exception);
std::ostream& operator<<(std::ostream& os, const Attributes& attributes);

typedef void (*FatalCallback)();
typedef void (*TraceCallback)(Level level, const std::string& message);
typedef void (*StreamCallback)(
  const Stream& stream, const void* input, void* output, int32_t frames, 
  double time, uint64_t position, bool timeValid, uint64_t error, void* user);

class Exception final: public std::exception {
private:
  const uint64_t e;

public:
  Exception(uint64_t e);
  uint64_t GetError() const;
  static Cause GetCause(uint64_t error);
  static System GetSystem(uint64_t error);
  static uint32_t GetFault(uint64_t error);
  static std::string GetText(uint64_t error);
  const char* what() const noexcept override;
};

class Print final {
private:
  Print();
public:

  static std::string LevelToString(Level level);
  static std::string CauseToString(Cause cause);
  static std::string SetupToString(Setup setup);
  static std::string MixToString(const Mix& mix);
  static std::string ErrorToString(uint64_t error);
  static std::string SystemToString(System system);
  static std::string SampleToString(Sample sample);
  static std::string FormatToString(const Format& format);
  static std::string BufferToString(const Buffer& buffer);
  static std::string LatencyToString(const Latency& latency);
  static std::string CapabilitiesToString(Capabilities capabilities);
  static std::string AttributesToString(const Attributes& attributes);
};

class Stream final {
private:
  XtStream* s;
  friend class Device;
  friend struct StreamCallbackForwarder;
  
  void* const user;
  const StreamCallback callback;
  Stream(StreamCallback callback, void* user);

public:
  ~Stream();
  void Stop();
  void Start();
  System GetSystem() const;
  int32_t GetFrames() const;
  bool IsInterleaved() const;
  Latency GetLatency() const;
  const Format& GetFormat() const;
};

class Service final {
private:
  friend class Audio;
  const XtService* const s;
  Service(const XtService* s);

public:
  System GetSystem() const;
  std::string GetName() const;
  int32_t GetDeviceCount() const;
  Capabilities GetCapabilities() const;
  std::unique_ptr<Device> OpenDevice(int32_t index) const;
  std::unique_ptr<Device> OpenDefaultDevice(bool output) const;
};

class Audio final {
public:
  ~Audio();
  Audio(const std::string& id, void* window, TraceCallback trace, FatalCallback fatal);

  static bool IsWin32();
  static std::string GetVersion();
  static int32_t GetServiceCount();
  static Attributes GetSampleAttributes(Sample sample);
  static std::unique_ptr<Service> GetServiceBySetup(Setup setup);
  static std::unique_ptr<Service> GetServiceByIndex(int32_t index);
  static std::unique_ptr<Service> GetServiceBySystem(System system);
};

class Device final {
private:
  friend class Service;
  XtDevice* const d;
  Device(XtDevice* d);

public:
  ~Device();
  void ShowControlPanel();
  System GetSystem() const;
  std::string GetName() const;
  std::unique_ptr<Mix> GetMix() const;
  int32_t GetChannelCount(bool output) const;
  bool SupportsAccess(bool interleaved) const;
  Buffer GetBuffer(const Format& format) const;
  bool SupportsFormat(const Format& format) const;
  std::string GetChannelName(bool output, int32_t index) const;
  std::unique_ptr<Stream> OpenStream(const Format& format, bool interleaved, 
                                     double bufferSize, StreamCallback callback, void* user);
};

} // namespace Xt
#endif // _XT_AUDIO_HPP
