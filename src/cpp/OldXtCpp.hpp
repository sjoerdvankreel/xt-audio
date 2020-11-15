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
  std::unique_ptr<Stream> AggregateStream(const AggregateStreamParams& params, void* user);
};

class Audio final {
  Audio() = default;
public:
  static Version GetVersion();
  static std::vector<System> GetSystems();
  static System SetupToSystem(Setup setup); 
  static ErrorInfo GetErrorInfo(uint64_t error);
  static Attributes GetSampleAttributes(Sample sample);
  static std::unique_ptr<Service> GetService(System system);
  static std::unique_ptr<void, void(*)(void*)> Init(const std::string& id, void* window, OnError onError);
};
} // namespace Xt
#endif // XT_CPP_HPP