#ifndef XT_API_DEVICE_HPP
#define XT_API_DEVICE_HPP

/** @file */
/** @cond */
#include <xt/cpp/Core.hpp>
#include <xt/cpp/Utility.hpp>
#include <xt/api/Structs.hpp>
#include <xt/api/XtStream.hpp>

#include <memory>
#include <vector>
#include <optional>
/** @endcond */

namespace Xt {

using namespace Detail;

class Device final
{
/** @cond */
  friend class Service;
/** @endcond */
  XtDevice* const _d;
  Device(XtDevice* d): _d(d) { }
public:
  ~Device();
  void ShowControlPanel();
  void* GetHandle() const;
  std::optional<Mix> GetMix() const;
  int32_t GetChannelCount(bool output) const;
  bool SupportsAccess(bool interleaved) const;
  bool SupportsFormat(Format const& format) const;
  BufferSize GetBufferSize(Format const& format) const;
  std::string GetChannelName(bool output, int32_t index) const;
  std::unique_ptr<Stream> OpenStream(DeviceStreamParams const& params, void* user);
};

inline
Device::~Device()
{ XtDeviceDestroy(_d); }
inline void*
Device::GetHandle() const
{ return XtDeviceGetHandle(_d); }
inline void 
Device::ShowControlPanel() 
{ Detail::HandleError(XtDeviceShowControlPanel(_d)); }

inline int32_t 
Device::GetChannelCount(bool output) const 
{ 
  int32_t count; 
  Detail::HandleError(XtDeviceGetChannelCount(_d, output, &count)); 
  return count; 
}

inline BufferSize 
Device::GetBufferSize(Format const& format) const 
{ 
  BufferSize result; 
  auto coreSize = reinterpret_cast<XtBufferSize*>(&result);
  auto coreFormat = reinterpret_cast<XtFormat const*>(&format);
  Detail::HandleError(XtDeviceGetBufferSize(_d, coreFormat, coreSize));
  return result;
}

inline bool 
Device::SupportsFormat(Format const& format) const 
{
  XtBool supports; 
  auto coreFormat = reinterpret_cast<XtFormat const*>(&format);
  Detail::HandleError(XtDeviceSupportsFormat(_d, coreFormat, &supports)); 
  return supports != XtFalse; 
}

inline bool 
Device::SupportsAccess(bool interleaved) const 
{
  XtBool supports;
  Detail::HandleError(XtDeviceSupportsAccess(_d, interleaved, &supports));
  return supports != XtFalse;    
}

inline std::optional<Mix> 
Device::GetMix() const 
{
  Mix mix;
  XtBool valid;
  auto coreMix = reinterpret_cast<XtMix*>(&mix);
  Detail::HandleError(XtDeviceGetMix(_d, &valid, coreMix));
  return valid? std::optional<Mix>(mix): std::optional<Mix>(std::nullopt);
}

inline std::string 
Device::GetChannelName(bool output, int32_t index) const
{
  int32_t size = 0;
  Detail::HandleError(XtDeviceGetChannelName(_d, output, index, nullptr, &size));
  std::vector<char> buffer(static_cast<size_t>(size));
  Detail::HandleError(XtDeviceGetChannelName(_d, output, index, buffer.data(), &size));
  return std::string(buffer.data());
}

inline std::unique_ptr<Stream> 
Device::OpenStream(DeviceStreamParams const& params, void* user) 
{
  XtStream* stream; 
  XtDeviceStreamParams coreParams = { 0 };
  coreParams.bufferSize = params.bufferSize;
  coreParams.stream.onBuffer = &Detail::ForwardOnBuffer;
  coreParams.stream.interleaved = params.stream.interleaved;
  coreParams.format = *reinterpret_cast<XtFormat const*>(&params.format);
  coreParams.stream.onXRun = params.stream.onXRun == nullptr? nullptr: &Detail::ForwardOnXRun;
  coreParams.stream.onRunning = params.stream.onRunning == nullptr? nullptr: &Detail::ForwardOnRunning;
  std::unique_ptr<Stream> result(new Stream(params.stream, user));
  Detail::HandleError(XtDeviceOpenStream(_d, &coreParams, result.get(), &stream));
  result->_s = stream;
  return result;
}

} // namespace Xt
#endif // XT_API_DEVICE_HPP