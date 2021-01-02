#if XT_ENABLE_PULSE
#include <xt/backend/pulse/Shared.hpp>
#include <pulse/pulseaudio.h>
#include <cstring>

XtFault
PulseService::GetFormatFault() const
{ return XT_PA_ERR_FORMAT; }

XtServiceCaps 
PulseService::GetCapabilities() const
{ 
  auto result = XtServiceCapsAggregation | XtServiceCapsChannelMask;
  return static_cast<XtServiceCaps>(result); 
}

XtFault
PulseService::OpenDeviceList(XtEnumFlags flags, XtDeviceList** list) const
{
  bool input = (flags & XtEnumFlagsInput) != 0;
  bool output = (flags & XtEnumFlagsOutput) != 0;
  *list = new PulseDeviceList(input, output);
  return PA_OK; 
}

XtFault
PulseService::OpenDevice(char const* id, XtDevice** device) const
{
  XtFault fault;
  XtPaSimple pa;
  XtBool output = strcmp(id, "0");
  if((fault = XtiCreatePulseDefaultClient(output, &pa.pa)) != PA_OK) return fault;
  *device = new PulseDevice(output);
  return PA_OK;
}

XtFault
PulseService::GetDefaultDeviceId(XtBool output, XtBool* valid, char* buffer, int32_t* size) const
{
  if(output) XtiCopyString("1", buffer, size);
  else XtiCopyString("0", buffer, size);
  *valid = XtTrue;
  return PA_OK;
}

#endif // XT_ENABLE_PULSE