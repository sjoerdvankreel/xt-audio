#ifndef XT_SERVICE_HPP
#define XT_SERVICE_HPP

#include "Forward.hpp"
#include "XtDevice.hpp"
#include "CoreEnums.hpp"
#include "CoreStructs.hpp"
#include <XtAudio.h>
#include <memory>

namespace Xt {

class Service final 
{
  friend class Audio;
  XtService const* const _s;
  Service(XtService const* s): _s(s) { }
public:
  int32_t GetDeviceCount() const;
  std::unique_ptr<Device> OpenDevice(int32_t index) const;
  std::unique_ptr<Device> OpenDefaultDevice(bool output) const;
  std::unique_ptr<Stream> AggregateStream(AggregateStreamParams const& params, void* user);
  Capabilities GetCapabilities() const { return static_cast<Capabilities>(XtServiceGetCapabilities(_s)); }
};

inline int32_t Service::GetDeviceCount() const 
{ 
  int32_t count; 
  Detail::HandleError(XtServiceGetDeviceCount(_s, &count));
  return count;
}

inline std::unique_ptr<Device> Service::OpenDevice(int32_t index) const 
{ 
  XtDevice* device; 
  Detail::HandleError(XtServiceOpenDevice(_s, index, &device));
  return std::unique_ptr<Device>(new Device(device));
}

inline std::unique_ptr<Device> Service::OpenDefaultDevice(bool output) const
{ 
  XtDevice* device; 
  Detail::HandleError(XtServiceOpenDefaultDevice(_s, output != XtFalse, &device));
  return device == nullptr? std::unique_ptr<Device>(): std::unique_ptr<Device>(new Device(device));
}

inline std::unique_ptr<Stream> Service::AggregateStream(AggregateStreamParams const& params, void* user)
{
  XtStream* stream; 
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
#endif // XT_SERVICE_HPP