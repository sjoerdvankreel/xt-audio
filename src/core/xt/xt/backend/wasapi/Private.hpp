#ifndef XT_BACKEND_WASAPI_PRIVATE_HPP
#define XT_BACKEND_WASAPI_PRIVATE_HPP
#if XT_ENABLE_WASAPI

#include <xt/api/Enums.h>
#include <xt/shared/Shared.hpp>

#include <mmdeviceapi.h>
#include <string>

enum class XtWasapiType
{
  Shared,
  Loopback,
  Exclusive
};

struct XtWasapiDeviceInfo
{
  std::string id;
  XtWasapiType type;
};

char const* 
XtiGetWasapiFaultText(XtFault fault);
XtCause 
XtiGetWasapiFaultCause(XtFault fault);
XtWasapiDeviceInfo
XtiParseWasapiDeviceInfo(std::string const& id);
std::string
XtiGetWasapiDeviceId(XtWasapiDeviceInfo const& info);
HRESULT
XtiGetWasapiDeviceInfo(IMMDevice* device, XtWasapiType type, XtWasapiDeviceInfo* result);

#endif // XT_ENABLE_WASAPI
#endif // XT_BACKEND_WASAPI_PRIVATE_HPP