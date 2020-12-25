#ifndef XT_BACKEND_WASAPI_PRIVATE_HPP
#define XT_BACKEND_WASAPI_PRIVATE_HPP
#if XT_ENABLE_WASAPI

#include <xt/api/Enums.h>
#include <xt/shared/Win32.hpp>
#include <xt/shared/Shared.hpp>

#include <mmdeviceapi.h>
#include <string>

enum class XtWasapiType
{
  SharedCapture,
  SharedRender,
  ExclusiveCapture,
  ExclusiveRender,
  Loopback
};

struct XtWasapiDeviceInfo
{
  std::string id;
  XtWasapiType type;
};

struct XtWsEvent
{
  HANDLE event;
  XtWsEvent(XtWsEvent const&) = delete;
  XtWsEvent& operator=(XtWsEvent const&) = delete;
  ~XtWsEvent() { XT_ASSERT(CloseHandle(event)); }
  XtWsEvent(): event() { XT_ASSERT((event = CreateEvent(nullptr, FALSE, FALSE, nullptr)) != nullptr); }
};

inline double const
XtiWasapiHnsPerMs = 10000.0;
inline double const
XtiWasapiMaxSharedBufferMs = 2000.0;
inline double const
XtiWasapiMaxExclusiveBufferMs = 500.0;

char const* 
XtiGetWasapiFaultText(XtFault fault);
XtCause 
XtiGetWasapiFaultCause(XtFault fault);
bool
XtiWasapiTypeIsOutput(XtWasapiType type);
char const*
XtiGetWasapiNameSuffix(XtWasapiType type);
int32_t
XtiGetWasapiDeviceCaps(XtWasapiType type);
bool
XtiWasapiTypeIsExclusive(XtWasapiType type);
XtWasapiDeviceInfo
XtiParseWasapiDeviceInfo(std::string const& id);
std::string
XtiGetWasapiDeviceId(XtWasapiDeviceInfo const& info);
HRESULT
XtiGetWasapiDeviceInfo(IMMDevice* device, XtWasapiType type, XtWasapiDeviceInfo* result);

#endif // XT_ENABLE_WASAPI
#endif // XT_BACKEND_WASAPI_PRIVATE_HPP