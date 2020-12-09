#if XT_ENABLE_PULSE
#include <xt/pulse/Shared.hpp>
#include <xt/private/Shared.hpp>
#include <pulse/pulseaudio.h>
#include <cstring>

XtFault
PulseDeviceList::GetCount(int32_t* count) const
{ *count = 2; return PA_OK; }

XtFault
PulseDeviceList::GetDefaultId(XtBool output, XtBool* valid, char* buffer, int32_t* size) const
{
  *valid = XtTrue;
  return GetId(output? 1: 0, buffer, size);
}
  
XtFault 
PulseDeviceList::GetId(int32_t index, char* buffer, int32_t* size) const
{  
  if(index == 0) XtiCopyString("0", buffer, size);
  else if(index == 1) XtiCopyString("1", buffer, size);
  else XT_ASSERT(false);
  return PA_OK;
}

XtFault
PulseDeviceList::GetName(char const* id, char* buffer, int32_t* size) const
{
  if(!strcmp(id, "0")) XtiCopyString("Input", buffer, size);
  else if(!strcmp(id, "1")) XtiCopyString("Output", buffer, size);
  else XT_ASSERT(false);
  return PA_OK;
}

#endif // XT_ENABLE_PULSE