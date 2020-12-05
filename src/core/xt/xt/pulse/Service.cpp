#if XT_ENABLE_PULSE
#include <xt/pulse/Shared.hpp>
#include <pulse/pulseaudio.h>

XtFault 
PulseService::OpenDevice(int32_t index, XtDevice** device) const
{ return OpenDefaultDevice(index != 0, device); }

XtCapabilities 
PulseService::GetCapabilities() const
{ 
  auto result = XtCapabilitiesAggregation | XtCapabilitiesChannelMask;
  return static_cast<XtCapabilities>(result); 
}

XtFault
PulseService::GetDeviceCount(int32_t* count) const
{
  XtFault fault;
  XtPaSimple pa;
  if((fault = XtiCreatePulseDefaultClient(XtTrue, &pa.pa)) != PA_OK) return fault;
  *count = pa.pa == nullptr? 0: 2;
  return PA_OK;
}

XtFault PulseService::OpenDefaultDevice(XtBool output, XtDevice** device) const {
  XtFault fault;
  XtPaSimple pa;
  if((fault = XtiCreatePulseDefaultClient(output, &pa.pa)) != PA_OK) return fault;
  *device = new PulseDevice(output);
  return PA_OK;
}

#endif // XT_ENABLE_PULSE