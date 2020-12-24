#ifndef XT_CPP_DEVICE_LIST_HPP
#define XT_CPP_DEVICE_LIST_HPP

/** @file */

#include <xt/cpp/Core.hpp>
#include <xt/cpp/Utility.hpp>
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
  void* GetHandle() const;
  int32_t GetCount() const;
  std::string GetId(int32_t index) const;
  std::string GetName(std::string const& id) const;
};

inline
DeviceList::~DeviceList()
{ XtDeviceListDestroy(_l); }
inline void*
DeviceList::GetHandle() const
{ return XtDeviceListGetHandle(_l); }

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

} // namespace Xt
#endif // XT_CPP_DEVICE_LIST_HPP