#ifndef XT_API_PRIVATE_SERVICE_HPP
#define XT_API_PRIVATE_SERVICE_HPP

#include <xt/api/public/Enums.h>
#include <xt/api/public/Shared.h>
#include <xt/private/Shared.hpp>
#include <cstdint>

#define XT_IMPLEMENT_SERVICE()                                         \
  XtSystem GetSystem() const override;                                 \
  XtCapabilities GetCapabilities() const override;                     \
  XtFault GetDeviceCount(int32_t* count) const override;               \
  XtFault OpenDevice(int32_t index, XtDevice** device) const override; \
  XtFault OpenDefaultDevice(XtBool output, XtDevice** device) const override;

struct XtService 
{
  virtual ~XtService() {};
  virtual XtSystem GetSystem() const = 0;
  virtual XtCapabilities GetCapabilities() const = 0;
  virtual XtFault GetDeviceCount(int32_t* count) const = 0;
  virtual XtFault OpenDevice(int32_t index, XtDevice** device) const = 0;
  virtual XtFault OpenDefaultDevice(XtBool output, XtDevice** device) const = 0;
};

#endif // XT_API_PRIVATE_SERVICE_HPP