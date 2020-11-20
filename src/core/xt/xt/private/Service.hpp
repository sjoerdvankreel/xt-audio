#ifndef XT_PRIVATE_SERVICE_HPP
#define XT_PRIVATE_SERVICE_HPP

#include <xt/audio/Enums.h>
#include <xt/audio/Shared.h>
#include <xt/private/Shared.hpp>
#include <cstdint>

#define XT_IMPLEMENT_SERVICE()                                         \
  XtSystem GetSystem() const override;                                 \
  XtFault GetFormatFault() const override;                             \
  XtCapabilities GetCapabilities() const override;                     \
  XtCause GetFaultCause(XtFault fault) const override;                 \
  XtFault GetDeviceCount(int32_t* count) const override;               \
  char const* GetFaultText(XtFault fault) const override;              \
  XtFault OpenDevice(int32_t index, XtDevice** device) const override; \
  XtFault OpenDefaultDevice(XtBool output, XtDevice** device) const override;

struct XtService 
{
  virtual ~XtService() {};
  virtual XtSystem GetSystem() const = 0;
  virtual XtFault GetFormatFault() const = 0;
  virtual XtCapabilities GetCapabilities() const = 0;
  virtual XtCause GetFaultCause(XtFault fault) const = 0;
  virtual XtFault GetDeviceCount(int32_t* count) const = 0;
  virtual char const* GetFaultText(XtFault fault) const = 0;
  virtual XtFault OpenDevice(int32_t index, XtDevice** device) const = 0;
  virtual XtFault OpenDefaultDevice(XtBool output, XtDevice** device) const = 0;
};

#endif // XT_PRIVATE_SERVICE_HPP