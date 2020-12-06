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
  int32_t GetCount() const;
  int32_t GetDefault(bool output) const;
  std::string GetId(int32_t index) const;
  std::string GetName(int32_t index) const;
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

inline int32_t
DeviceList::GetDefault(bool output) const
{
  int32_t result; 
  Detail::HandleError(XtDeviceListGetDefault(_l, output, &result));
  return result;
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
DeviceList::GetName(int32_t index) const 
{ 
  int32_t size = 0;
  Detail::HandleError(XtDeviceListGetName(_l, index, nullptr, &size));
  std::vector<char> buffer(static_cast<size_t>(size));
  Detail::HandleError(XtDeviceListGetName(_l, index, buffer.data(), &size));
  return std::string(buffer.data());
}

} // namespace Xt
#endif // XT_CPP_DEVICE_LIST_HPP