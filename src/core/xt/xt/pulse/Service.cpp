#if XT_ENABLE_PULSE
#include <xt/pulse/Shared.hpp>
#include <pulse/pulseaudio.h>
#include <cstring>

XtFault
PulseService::OpenDeviceList(XtDeviceList** list) const
{
  *list = new PulseDeviceList;
  return PA_OK; 
}

XtCapabilities 
PulseService::GetCapabilities() const
{ 
  auto result = XtCapabilitiesAggregation | XtCapabilitiesChannelMask;
  return static_cast<XtCapabilities>(result); 
}

XtFault
PulseService::OpenDevice(char const* id, XtDevice** device) const
{
  XtFault fault;
  XtPaSimple pa;
  XtBool output = strcmp(id, "Output") == 0;
  if((fault = XtiCreatePulseDefaultClient(output, &pa.pa)) != PA_OK) return fault;
  *device = new PulseDevice(output);
  return PA_OK;
}

#endif // XT_ENABLE_PULSE