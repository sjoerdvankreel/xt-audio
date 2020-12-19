#ifndef XT_PRIVATE_DEVICE_LIST_HPP
#define XT_PRIVATE_DEVICE_LIST_HPP

#include <xt/api/Enums.h>
#include <xt/api/Shared.h>
#include <xt/shared/Shared.hpp>

#define XT_IMPLEMENT_DEVICE_LIST(s)                                               \
  XtFault GetCount(int32_t* count) const override final;                          \
  XtSystem GetSystem() const override final { return XtSystem##s; }               \
  XtFault GetId(int32_t index, char* buffer, int32_t* size) const override final; \
  XtFault GetName(char const* id, char* buffer, int32_t* size) const override final

struct XtDeviceList
{
  virtual ~XtDeviceList() { };  
  virtual XtSystem GetSystem() const = 0;
  virtual XtFault GetCount(int32_t* count) const = 0;
  virtual XtFault GetId(int32_t index, char* buffer, int32_t* size) const = 0;
  virtual XtFault GetName(char const* id, char* buffer, int32_t* size) const = 0;
};

#endif // XT_PRIVATE_DEVICE_LIST_HPP