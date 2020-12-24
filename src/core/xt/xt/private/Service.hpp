#ifndef XT_PRIVATE_SERVICE_HPP
#define XT_PRIVATE_SERVICE_HPP

#include <xt/api/Enums.h>
#include <xt/api/Shared.h>
#include <xt/api/Structs.h>
#include <xt/api/XtStream.h>
#include <xt/api/XtDevice.h>
#include <xt/shared/Shared.hpp>
#include <xt/api/XtDeviceList.h>

#include <cstdint>

#define XT_IMPLEMENT_SERVICE(s)                                                        \
  XtFault GetFormatFault() const override final;                                       \
  XtServiceCaps GetCapabilities() const override final;                                \
  XtSystem GetSystem() const override final { return XtSystem##s; }                    \
  XtFault OpenDevice(char const* id, XtDevice** device) const override final;          \
  XtFault OpenDeviceList(XtEnumFlags flags, XtDeviceList** list) const override final; \
  XtFault GetDefaultDeviceId(XtBool output, XtBool* valid, char* buffer, int32_t* size) const override final

struct XtService 
{
  virtual ~XtService() {};
  virtual XtSystem GetSystem() const = 0;
  virtual XtFault GetFormatFault() const = 0;
  virtual XtServiceCaps GetCapabilities() const = 0;
  virtual XtFault OpenDevice(char const* id, XtDevice** device) const = 0;
  virtual XtFault OpenDeviceList(XtEnumFlags flags, XtDeviceList** list) const = 0;
  XtFault AggregateStream(XtAggregateStreamParams const* params, void* user, XtStream** stream) const;
  virtual XtFault GetDefaultDeviceId(XtBool output, XtBool* valid, char* buffer, int32_t* size) const = 0;
};

#endif // XT_PRIVATE_SERVICE_HPP