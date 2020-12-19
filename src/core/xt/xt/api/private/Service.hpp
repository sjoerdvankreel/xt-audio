#ifndef XT_API_PRIVATE_SERVICE_HPP
#define XT_API_PRIVATE_SERVICE_HPP

#include <xt/api/public/Enums.h>
#include <xt/api/public/Shared.h>
#include <xt/api/public/Structs.h>
#include <xt/api/public/XtStream.h>
#include <xt/api/public/XtDevice.h>
#include <xt/api/public/XtDeviceList.h>
#include <xt/private/Shared.hpp>
#include <cstdint>

#define XT_IMPLEMENT_SERVICE(s)                                                        \
  XtCapabilities GetCapabilities() const override final;                               \
  XtSystem GetSystem() const override final { return XtSystem##s; }                    \
  XtFault OpenDevice(char const* id, XtDevice** device) const override final;          \
  XtFault OpenDeviceList(XtEnumFlags flags, XtDeviceList** list) const override final; \
  XtFault GetDefaultDeviceId(XtBool output, XtBool* valid, char* buffer, int32_t* size) const override final

struct XtService 
{
  virtual ~XtService() {};
  virtual XtSystem GetSystem() const = 0;
  virtual XtCapabilities GetCapabilities() const = 0;
  virtual XtFault OpenDevice(char const* id, XtDevice** device) const = 0;
  virtual XtFault OpenDeviceList(XtEnumFlags flags, XtDeviceList** list) const = 0;
  XtFault AggregateStream(XtAggregateStreamParams const* params, void* user, XtStream** stream) const;
  virtual XtFault GetDefaultDeviceId(XtBool output, XtBool* valid, char* buffer, int32_t* size) const = 0;
};

#endif // XT_API_PRIVATE_SERVICE_HPP