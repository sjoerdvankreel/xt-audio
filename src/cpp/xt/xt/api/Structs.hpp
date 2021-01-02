#ifndef XT_API_STRUCTS_HPP
#define XT_API_STRUCTS_HPP

/** @file */
/** @cond */
#include <xt/api/Enums.hpp>
#include <xt/api/Callbacks.hpp>

#include <string>
#include <cstdint>
/** @endcond */

namespace Xt {

struct Buffer final 
{
  void const* input;
  void* output;
  double time;
  uint64_t position;
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

struct Location final
{
  std::string file;
  std::string func;
  int32_t line;
};

struct ServiceError final
{
  Cause cause;
  std::string text;
};

struct ErrorInfo  final
{
  uint32_t fault;
  System system;
  ServiceError service;
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
  OnRunning onRunning;
  StreamParams() = default;
  StreamParams(bool interleaved, OnBuffer onBuffer, OnXRun onXRun, OnRunning onRunning):
  interleaved(interleaved), onBuffer(onBuffer), onXRun(onXRun), onRunning(onRunning) {}
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
#endif // XT_API_STRUCTS_HPP