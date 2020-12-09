#if XT_ENABLE_JACK
#include <xt/jack/Shared.hpp>
#include <xt/jack/Private.hpp>
#include <memory>

JackService::
JackService()
{ jack_set_error_function(&XtiJackErrorCallback); }
JackService::
~JackService()
{ jack_set_error_function(&XtiJackSilentCallback); }

XtCapabilities
JackService::GetCapabilities() const 
{
  auto result = XtCapabilitiesTime
    | XtCapabilitiesFullDuplex
    | XtCapabilitiesChannelMask
    | XtCapabilitiesXRunDetection;
  return static_cast<XtCapabilities>(result);
}

XtFault
JackService::OpenDeviceList(XtDeviceList** list) const
{
  *list = new JackDeviceList;
  return 0;
}

XtFault
JackService::OpenDevice(char const* id, XtDevice** device) const
{  
  auto id = XtPlatform::instance->_id.c_str();
  XtJackClient jc(jack_client_open(id, JackNullOption, nullptr));
  if(jc.jc == nullptr) return ESRCH;
  auto result = std::make_unique<JackDevice>();
  result->_jc = std::move(jc);
  *device = result.release();
  return 0;
}

#endif // XT_ENABLE_JACK