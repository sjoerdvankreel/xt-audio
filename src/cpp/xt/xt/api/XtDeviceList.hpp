#ifndef XT_API_DEVICE_LIST_HPP
#define XT_API_DEVICE_LIST_HPP

/** @file */

#include <xt/api/Core.hpp>
#include <xt/api/Utility.hpp>

#include <string>
#include <vector>

namespace Xt {

using namespace Detail;

class DeviceList final
{
  friend class Service;
  XtDeviceList* const _l;
  DeviceList(XtDeviceList* l): _l(l) { }
public:
  ~DeviceList();
  int32_t GetCount() const;
  std::string GetId(int32_t index) const;
  std::string GetName(std::string const& id) const;
  DeviceCaps GetCapabilities(std::string const& id) const;
};

inline
DeviceList::~DeviceList()
{ XtDeviceListDestroy(_l); }

inline int32_t
DeviceList::GetCount() const
{
  int32_t count; 
  Detail::HandleError(XtDeviceListGetCount(_l, &count));
  return count;
}

inline std::string
DeviceList::GetId(int32_t index) const 
{ 
  int32_t size = 0;
  Detail::HandleError(XtDeviceListGetId(_l, index, nullptr, &size));
  std::vector<char> buffer(static_cast<size_t>(size));
  Detail::HandleError(XtDeviceListGetId(_l, index, buffer.data(), &size));
  return std::string(buffer.data());
}

inline std::string
DeviceList::GetName(std::string const& id) const 
{ 
  int32_t size = 0;
  Detail::HandleError(XtDeviceListGetName(_l, id.c_str(), nullptr, &size));
  std::vector<char> buffer(static_cast<size_t>(size));
  Detail::HandleError(XtDeviceListGetName(_l, id.c_str(), buffer.data(), &size));
  return std::string(buffer.data());
}

inline DeviceCaps
DeviceList::GetCapabilities(std::string const& id) const
{
  XtDeviceCaps coreCapabilities; 
  Detail::HandleError(XtDeviceListGetCapabilities(_l, id.c_str(), &coreCapabilities));
  return static_cast<DeviceCaps>(coreCapabilities);
}

} // namespace Xt
#endif // XT_API_DEVICE_LIST_HPP