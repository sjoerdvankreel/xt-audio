#if XT_ENABLE_PULSE
#include <xt/shared/Shared.hpp>
#include <xt/backend/pulse/Shared.hpp>

#include <pulse/pulseaudio.h>
#include <cstring>

PulseDeviceList::
PulseDeviceList(bool input, bool output):
_input(input), _output(output) { }

XtFault
PulseDeviceList::GetCount(int32_t* count) const
{
  *count = (_input? 1: 0) + (_output? 1: 0);
  return PA_OK;
}
  
XtFault 
PulseDeviceList::GetId(int32_t index, char* buffer, int32_t* size) const
{  
  if(index == 0 && _input) XtiCopyString("0", buffer, size);
  else XtiCopyString("1", buffer, size);
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