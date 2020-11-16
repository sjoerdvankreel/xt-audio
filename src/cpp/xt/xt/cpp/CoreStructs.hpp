#ifndef XT_CORE_STRUCTS_HPP
#define XT_CORE_STRUCTS_HPP

#include <xt/cpp/CoreEnums.hpp>
#include <xt/cpp/CoreCallbacks.hpp>

#include <cstdint>

namespace Xt {

struct Buffer final 
{
  void const* input;
  void* output;
  double time;
  uint64_t position;
  uint64_t error;
  int32_t frames;
  bool timeValid;
};

struct Latency final 
{
  double input;
  double output;
};

struct Version final 
{
  int32_t major;
  int32_t minor;
};

struct ErrorInfo final 
{
  System system;
  Cause cause;
  std::string text;
  uint32_t fault;
};

struct BufferSize final 
{
  double min;
  double max;
  double current;
};

struct Attributes final 
{
  int32_t size;
  int32_t count;
  bool isFloat;
  bool isSigned;
};

struct Mix final 
{
  int32_t rate;
  Sample sample;
  Mix() = default;
  Mix(int32_t rate, Sample sample): 
  rate(rate), sample(sample) {}
};

struct Channels final 
{
  int32_t inputs;
  uint64_t inMask;
  int32_t outputs;
  uint64_t outMask;
  Channels() = default;
  Channels(int32_t inputs, uint64_t inMask, int32_t outputs, uint64_t outMask):
  inputs(inputs), inMask(inMask), outputs(outputs), outMask(outMask) {}
};

struct Format final 
{
  Mix mix;
  Channels channels;  
  Format() = default;
  Format(Mix const& mix, Channels const& channels): 
  mix(mix), channels(channels) {}
};

struct StreamParams final 
{
  bool interleaved;
  OnBuffer onBuffer;
  OnXRun onXRun;
  StreamParams() = default;
  StreamParams(bool interleaved, OnBuffer onBuffer, OnXRun onXRun):
  interleaved(interleaved), onBuffer(onBuffer), onXRun(onXRun) {}
};

struct DeviceStreamParams final 
{
  StreamParams stream;
  Format format;
  double bufferSize;
  DeviceStreamParams() = default;
  DeviceStreamParams(StreamParams const& stream, Format const& format, double bufferSize):
  stream(stream), format(format), bufferSize(bufferSize) {}
};

struct AggregateDeviceParams final 
{
  class Device* device;
  Channels channels;
  double bufferSize;
  AggregateDeviceParams() = default;
  AggregateDeviceParams(class Device* device, Channels const& channels, double bufferSize):
  device(device), channels(channels), bufferSize(bufferSize) {}
};

struct AggregateStreamParams final 
{
  StreamParams stream;
  AggregateDeviceParams *devices;
  int32_t count;
  Mix mix;
  Device const* master;
  AggregateStreamParams() = default;
  AggregateStreamParams(StreamParams const& stream, AggregateDeviceParams* devices, int32_t count, Mix const& mix, Device const* master):
  stream(stream), devices(devices), count(count), mix(mix), master(master) {}
};

} // namespace Xt
#endif // XT_CORE_STRUCTS_HPP