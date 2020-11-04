#ifndef XT_CPP_HPP
#define XT_CPP_HPP

/** @file */

#include <memory>
#include <string>
#include <vector>
#include <ostream>
#include <optional>
#include <stdexcept>

struct XtStream;
struct XtDevice;
struct XtService;

namespace Xt {

class Stream;
class Device;
class Service;
class Exception;

enum class Cause {
  Format,
  Service,
  Generic,
  Unknown,
  Endpoint
};

enum class System {
  ALSA = 1,
  ASIO,
  JACK,
  WASAPI,
  PulseAudio,
  DirectSound
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
  CapabilitiesChannelMask = 0x8,
  CapabilitiesXRunDetection = 0x10
};

struct Buffer final {
  const void* input;
  void* output;
  int32_t frames;
};

struct Time final {
  double time;
  uint64_t position;
  bool valid;  
};

struct BufferSize final {
  double min;
  double max;
  double current;
};

struct Version final {
  int32_t major;
  int32_t minor;
};

struct Latency final {
  double input;
  double output;
};

struct ErrorInfo final {
  System system;
  Cause cause;
  std::string text;
  uint32_t fault;
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
  Mix(int32_t rate, Sample sample): rate(rate), sample(sample) {}
};

struct Channels final {
  int32_t inputs;
  uint64_t inMask;
  int32_t outputs;
  uint64_t outMask;
  Channels() = default;
  Channels(int32_t inputs, uint64_t inMask, int32_t outputs, uint64_t outMask):
  inputs(inputs), inMask(inMask), outputs(outputs), outMask(outMask) {}
};

struct Format final {
  Mix mix;
  Channels channels;  
  Format() = default;
  Format(const Mix& mix, const Channels& channels): mix(mix), channels(channels) {}
};

std::ostream& operator<<(std::ostream& os, Cause cause);
std::ostream& operator<<(std::ostream& os, Setup setup);
std::ostream& operator<<(std::ostream& os, System system);
std::ostream& operator<<(std::ostream& os, Sample sample);
std::ostream& operator<<(std::ostream& os, const Device& device);
std::ostream& operator<<(std::ostream& os, const ErrorInfo& info);
std::ostream& operator<<(std::ostream& os, Capabilities capabilities);

typedef void (*XRunCallback)(int32_t index, void* user);
typedef void (*ErrorCallback)(const std::string& location, const std::string& message);
typedef void (*StreamCallback)(const Stream& stream, const Buffer& buffer, const Time& time, uint64_t error, void* user);

class Exception final: public std::exception {
  const uint64_t _error;
public:
  Exception(uint64_t error): _error(error) {}
  uint64_t GetError() const { return _error; }
};

class Stream final {
private:
  XtStream* s;
  friend class Device;
  friend class Service;
  friend struct StreamCallbackForwarder;

  void* const user;
  const XRunCallback xRunCallback;
  const StreamCallback streamCallback;
  Stream(StreamCallback streamCallback, XRunCallback xRunCallback, void* user);

public:
  ~Stream();
  void Stop();
  void Start();
  int32_t GetFrames() const;
  Latency GetLatency() const;
  const Format& GetFormat() const;
};

class Service final {
private:
  friend class Audio;
  const XtService* const s;
  Service(const XtService* s);

public:
  int32_t GetDeviceCount() const;
  Capabilities GetCapabilities() const;
  std::unique_ptr<Device> OpenDevice(int32_t index) const;
  std::unique_ptr<Device> OpenDefaultDevice(bool output) const;
  std::unique_ptr<Stream> AggregateStream(Device** devices, const Channels* channels, 
                                          const double* bufferSizes, int32_t count, 
                                          const Mix& mix, bool interleaved, Device& master, 
                                          StreamCallback streamCallback, XRunCallback xRunCallback, void* user);
};

class Audio final {
public:
  ~Audio();
  Audio(const std::string& id, void* window, ErrorCallback callback);

  static Version GetVersion();
  static std::vector<System> GetSystems();
  static System SetupToSystem(Setup setup); 
  static ErrorInfo GetErrorInfo(uint64_t error);
  static Attributes GetSampleAttributes(Sample sample);
  static std::unique_ptr<Service> GetService(System system);
};

class Device final {
private:
  friend class Service;
  XtDevice* const d;
  Device(XtDevice* d);

public:
  ~Device();
  void ShowControlPanel();
  std::string GetName() const;
  std::optional<Mix> GetMix() const;
  int32_t GetChannelCount(bool output) const;
  bool SupportsAccess(bool interleaved) const;
  bool SupportsFormat(const Format& format) const;
  BufferSize GetBufferSize(const Format& format) const;
  std::string GetChannelName(bool output, int32_t index) const;
  std::unique_ptr<Stream> OpenStream(const Format& format, bool interleaved, double bufferSize, 
                                     StreamCallback streamCallback, XRunCallback xRunCallback, void* user);
};

} // namespace Xt
#endif // XT_CPP_HPP