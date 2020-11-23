#ifndef XT_CPP_SERVICE_HPP
#define XT_CPP_SERVICE_HPP

/** @file */

#include <xt/cpp/Core.hpp>
#include <xt/cpp/Enums.hpp>
#include <xt/cpp/Structs.hpp>
#include <xt/cpp/Forward.hpp>
#include <xt/cpp/XtDevice.hpp>

#include <memory>

namespace Xt {

using namespace Detail;

class Service final 
{
  friend class Platform;
  XtService const* const _s;
  Service(XtService const* s): _s(s) { }
public:
  int32_t GetDeviceCount() const;
  Capabilities GetCapabilities() const;
  std::unique_ptr<Device> OpenDevice(int32_t index) const;
  std::unique_ptr<Device> OpenDefaultDevice(bool output) const;
  std::unique_ptr<Stream> AggregateStream(AggregateStreamParams const& params, void* user);
};

inline Capabilities
Service::GetCapabilities() const
{
  auto coreCapabilities = XtServiceGetCapabilities(_s);
  return static_cast<Capabilities>(coreCapabilities); 
}

inline int32_t 
Service::GetDeviceCount() const 
{ 
  int32_t count; 
  Detail::HandleError(XtServiceGetDeviceCount(_s, &count));
  return count;
}

inline std::unique_ptr<Device> 
Service::OpenDevice(int32_t index) const 
{ 
  XtDevice* device; 
  Detail::HandleError(XtServiceOpenDevice(_s, index, &device));
  return std::unique_ptr<Device>(new Device(device));
}

inline std::unique_ptr<Device> 
Service::OpenDefaultDevice(bool output) const
{ 
  XtDevice* device; 
  Detail::HandleError(XtServiceOpenDefaultDevice(_s, output != XtFalse, &device));
  if(device == nullptr) return std::unique_ptr<Device>();
  return std::unique_ptr<Device>(new Device(device));
}

inline std::unique_ptr<Stream> 
Service::AggregateStream(AggregateStreamParams const& params, void* user)
{
  XtStream* stream = nullptr;
  std::vector<XtAggregateDeviceParams> ds(params.count);
  for(int32_t i = 0; i < params.count; i++)
  {
    ds[i].device = params.devices[i].device->_d;
    ds[i].bufferSize = params.devices[i].bufferSize;
    ds[i].channels = *reinterpret_cast<XtChannels const*>(&params.devices[i].channels);
  }
  XtAggregateStreamParams coreParams = { 0 };
  coreParams.devices = ds.data();
  coreParams.count = params.count;
  coreParams.master = params.master->_d;
  coreParams.stream.onBuffer = Detail::ForwardOnBuffer;
  coreParams.stream.interleaved = params.stream.interleaved;
  coreParams.mix = *reinterpret_cast<XtMix const*>(&params.mix);
  coreParams.stream.onXRun = params.stream.onXRun == nullptr? nullptr: Detail::ForwardOnXRun;
  std::unique_ptr<Stream> result(new Stream(params.stream.onBuffer, params.stream.onXRun, user));
  Detail::HandleError(XtServiceAggregateStream(_s, &coreParams, result.get(), &stream));
  result->_s = stream;
  return result;
}

} // namespace Xt
#endif // XT_CPP_SERVICE_HPP