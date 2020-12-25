#if XT_ENABLE_WASAPI
#include <xt/shared/Win32.hpp>
#include <xt/backend/wasapi/Shared.hpp>
#include <xt/backend/wasapi/Private.hpp>

#include <memory>
#include <sstream>

std::unique_ptr<XtService>
XtiCreateWasapiService()
{ return std::make_unique<WasapiService>(); }

XtServiceError
XtiGetWasapiError(XtFault fault)
{
  XtServiceError result;
  result.text = XtiGetWasapiFaultText(fault);
  result.cause = XtiGetWasapiFaultCause(fault);
  return result;
}

bool
XtiWasapiTypeIsExclusive(XtWasapiType type)
{
  switch(type)
  {
  case XtWasapiType::Loopback:
  case XtWasapiType::SharedRender:
  case XtWasapiType::SharedCapture: return false;
  case XtWasapiType::ExclusiveRender:
  case XtWasapiType::ExclusiveCapture: return true;
  default: XT_ASSERT(false); return false;
  }
}

bool
XtiWasapiTypeIsOutput(XtWasapiType type)
{
  switch(type)
  {
  case XtWasapiType::SharedRender:
  case XtWasapiType::ExclusiveRender: return true;
  case XtWasapiType::Loopback:
  case XtWasapiType::SharedCapture:
  case XtWasapiType::ExclusiveCapture: return false;
  default: XT_ASSERT(false); return false;
  }
}

char const*
XtiGetWasapiNameSuffix(XtWasapiType type)
{
  switch(type)
  {
  case XtWasapiType::Loopback: return "Loopback";
  case XtWasapiType::SharedRender:
  case XtWasapiType::SharedCapture: return "Shared";
  case XtWasapiType::ExclusiveRender:
  case XtWasapiType::ExclusiveCapture: return "Exclusive";
  default: return XT_ASSERT(false), nullptr;
  }
}

XtWasapiDeviceInfo
XtiParseWasapiDeviceInfo(std::string const& id)
{
  XtWasapiDeviceInfo result;
  result.id = id;
  result.id.erase(id.size() - 4, 4);
  std::string type = std::string(1, id[id.length() - 2]);
  result.type = static_cast<XtWasapiType>(std::stoi(type));
  return result;
}

std::string
XtiGetWasapiDeviceId(XtWasapiDeviceInfo const& info)
{
  std::ostringstream sstream;
  sstream << info.id.c_str();
  sstream << ".{" << static_cast<int32_t>(info.type) << "}";
  return sstream.str();
}

HRESULT
XtiGetWasapiDeviceInfo(IMMDevice* device, XtWasapiType type, XtWasapiDeviceInfo* result)
{
  HRESULT hr;
  CComHeapPtr<wchar_t> id;
  XT_VERIFY_COM(device->GetId(&id));
  result->type = type;
  result->id = XtiWideStringToUtf8(id);
  return S_OK;
}

int32_t
XtiGetWasapiDeviceCaps(XtWasapiType type)
{
  switch(type)
  {
  case XtWasapiType::SharedRender: return XtDeviceCapsOutput;
  case XtWasapiType::SharedCapture: return XtDeviceCapsInput;
  case XtWasapiType::Loopback: return XtDeviceCapsInput | XtDeviceCapsLoopback;
  case XtWasapiType::ExclusiveRender: return XtDeviceCapsOutput | XtDeviceCapsHwDirect;
  case XtWasapiType::ExclusiveCapture: return XtDeviceCapsInput | XtDeviceCapsHwDirect;
  default: return XT_ASSERT(false), 0;
  }
}

XtCause 
XtiGetWasapiFaultCause(XtFault fault)
{
  switch(fault) 
  {
  case AUDCLNT_E_CPUUSAGE_EXCEEDED: return XtCauseGeneric;
  case AUDCLNT_E_UNSUPPORTED_FORMAT: return XtCauseFormat;
  case AUDCLNT_E_SERVICE_NOT_RUNNING: return XtCauseService;
  case AUDCLNT_E_DEVICE_IN_USE:
  case AUDCLNT_E_DEVICE_INVALIDATED:
  case AUDCLNT_E_ENGINE_FORMAT_LOCKED:
  case AUDCLNT_E_RESOURCES_INVALIDATED:
  case AUDCLNT_E_ENGINE_PERIODICITY_LOCKED:
  case AUDCLNT_E_EXCLUSIVE_MODE_NOT_ALLOWED: return XtCauseEndpoint;
  default: return XtCauseUnknown;
  }
}

char const* 
XtiGetWasapiFaultText(XtFault fault)
{
  switch(fault) 
  {
    case AUDCLNT_S_BUFFER_EMPTY: return XT_STRINGIFY(AUDCLNT_S_BUFFER_EMPTY);    
    case AUDCLNT_S_POSITION_STALLED: return XT_STRINGIFY(AUDCLNT_S_POSITION_STALLED);
    case AUDCLNT_S_THREAD_ALREADY_REGISTERED: return XT_STRINGIFY(AUDCLNT_S_THREAD_ALREADY_REGISTERED);
    case AUDCLNT_E_NOT_STOPPED: return XT_STRINGIFY(AUDCLNT_E_NOT_STOPPED);
    case AUDCLNT_E_BUFFER_ERROR: return XT_STRINGIFY(AUDCLNT_E_BUFFER_ERROR);
    case AUDCLNT_E_OUT_OF_ORDER: return XT_STRINGIFY(AUDCLNT_E_OUT_OF_ORDER);
    case AUDCLNT_E_INVALID_SIZE: return XT_STRINGIFY(AUDCLNT_E_INVALID_SIZE);
    case AUDCLNT_E_DEVICE_IN_USE: return XT_STRINGIFY(AUDCLNT_E_DEVICE_IN_USE);
    case AUDCLNT_E_NOT_INITIALIZED: return XT_STRINGIFY(AUDCLNT_E_NOT_INITIALIZED);
    case AUDCLNT_E_BUFFER_TOO_LARGE: return XT_STRINGIFY(AUDCLNT_E_BUFFER_TOO_LARGE);
    case AUDCLNT_E_BUFFER_SIZE_ERROR: return XT_STRINGIFY(AUDCLNT_E_BUFFER_SIZE_ERROR);
    case AUDCLNT_E_OFFLOAD_MODE_ONLY: return XT_STRINGIFY(AUDCLNT_E_OFFLOAD_MODE_ONLY);
    case AUDCLNT_E_CPUUSAGE_EXCEEDED: return XT_STRINGIFY(AUDCLNT_E_CPUUSAGE_EXCEEDED);
    case AUDCLNT_E_UNSUPPORTED_FORMAT: return XT_STRINGIFY(AUDCLNT_E_UNSUPPORTED_FORMAT);
    case AUDCLNT_E_DEVICE_INVALIDATED: return XT_STRINGIFY(AUDCLNT_E_DEVICE_INVALIDATED);
    case AUDCLNT_E_EVENTHANDLE_NOT_SET: return XT_STRINGIFY(AUDCLNT_E_EVENTHANDLE_NOT_SET);
    case AUDCLNT_E_INVALID_STREAM_FLAG: return XT_STRINGIFY(AUDCLNT_E_INVALID_STREAM_FLAG);
    case AUDCLNT_E_EXCLUSIVE_MODE_ONLY: return XT_STRINGIFY(AUDCLNT_E_EXCLUSIVE_MODE_ONLY);
    case AUDCLNT_E_ALREADY_INITIALIZED: return XT_STRINGIFY(AUDCLNT_E_ALREADY_INITIALIZED);
    case AUDCLNT_E_SERVICE_NOT_RUNNING: return XT_STRINGIFY(AUDCLNT_E_SERVICE_NOT_RUNNING);
    case AUDCLNT_E_WRONG_ENDPOINT_TYPE: return XT_STRINGIFY(AUDCLNT_E_WRONG_ENDPOINT_TYPE);
    case AUDCLNT_E_RAW_MODE_UNSUPPORTED: return XT_STRINGIFY(AUDCLNT_E_RAW_MODE_UNSUPPORTED);
    case AUDCLNT_E_NONOFFLOAD_MODE_ONLY: return XT_STRINGIFY(AUDCLNT_E_NONOFFLOAD_MODE_ONLY);
    case AUDCLNT_E_ENGINE_FORMAT_LOCKED: return XT_STRINGIFY(AUDCLNT_E_ENGINE_FORMAT_LOCKED);
    case AUDCLNT_E_HEADTRACKING_ENABLED: return XT_STRINGIFY(AUDCLNT_E_HEADTRACKING_ENABLED);
    case AUDCLNT_E_RESOURCES_INVALIDATED: return XT_STRINGIFY(AUDCLNT_E_RESOURCES_INVALIDATED);
    case AUDCLNT_E_INVALID_DEVICE_PERIOD: return XT_STRINGIFY(AUDCLNT_E_INVALID_DEVICE_PERIOD);
    case AUDCLNT_E_THREAD_NOT_REGISTERED: return XT_STRINGIFY(AUDCLNT_E_THREAD_NOT_REGISTERED);
    case AUDCLNT_E_INCORRECT_BUFFER_SIZE: return XT_STRINGIFY(AUDCLNT_E_INCORRECT_BUFFER_SIZE);
    case AUDCLNT_E_ENDPOINT_CREATE_FAILED: return XT_STRINGIFY(AUDCLNT_E_ENDPOINT_CREATE_FAILED);
    case AUDCLNT_E_BUFFER_SIZE_NOT_ALIGNED: return XT_STRINGIFY(AUDCLNT_E_BUFFER_SIZE_NOT_ALIGNED);
    case AUDCLNT_E_OUT_OF_OFFLOAD_RESOURCES: return XT_STRINGIFY(AUDCLNT_E_OUT_OF_OFFLOAD_RESOURCES);
    case AUDCLNT_E_BUFFER_OPERATION_PENDING: return XT_STRINGIFY(AUDCLNT_E_BUFFER_OPERATION_PENDING);
    case AUDCLNT_E_EVENTHANDLE_NOT_EXPECTED: return XT_STRINGIFY(AUDCLNT_E_EVENTHANDLE_NOT_EXPECTED);
    case AUDCLNT_E_HEADTRACKING_UNSUPPORTED: return XT_STRINGIFY(AUDCLNT_E_HEADTRACKING_UNSUPPORTED);
    case AUDCLNT_E_ENGINE_PERIODICITY_LOCKED: return XT_STRINGIFY(AUDCLNT_E_ENGINE_PERIODICITY_LOCKED);
    case AUDCLNT_E_EXCLUSIVE_MODE_NOT_ALLOWED: return XT_STRINGIFY(AUDCLNT_E_EXCLUSIVE_MODE_NOT_ALLOWED);
    case AUDCLNT_E_BUFDURATION_PERIOD_NOT_EQUAL: return XT_STRINGIFY(AUDCLNT_E_BUFDURATION_PERIOD_NOT_EQUAL);
    case AUDCLNT_E_ENDPOINT_OFFLOAD_NOT_CAPABLE: return XT_STRINGIFY(AUDCLNT_E_ENDPOINT_OFFLOAD_NOT_CAPABLE);
    default: return "Unknown fault.";
  }
}

#endif // XT_ENABLE_WASAPI