#if XT_ENABLE_JACK
#include <xt/jack/Shared.hpp>
#include <xt/jack/Private.hpp>
#include <xt/api/private/Platform.hpp>
#include <memory>

JackService::
JackService()
{ jack_set_error_function(&XtiJackErrorCallback); }
JackService::
~JackService()
{ jack_set_error_function(&XtiJackSilentCallback); }


XtFault
JackService::OpenDeviceList(XtEnumFlags flags, XtDeviceList** list) const
{ *list = new JackDeviceList; return 0; }

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
JackService::OpenDevice(char const* id, XtDevice** device) const
{  
  auto appId = XtPlatform::instance->_id.c_str();
  XtJackClient jc(jack_client_open(appId, JackNoStartServer, nullptr));
  if(jc.jc == nullptr) return ESRCH;
  auto result = std::make_unique<JackDevice>(std::move(jc));
  *device = result.release();
  return 0;
}

XtFault
JackService::GetDefaultDeviceId(XtBool output, XtBool* valid, char* buffer, int32_t* size) const
{
  *valid = XtTrue;
  XtiCopyString("0", buffer, size);
  return 0; 
}

#endif // XT_ENABLE_JACK