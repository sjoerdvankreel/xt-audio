#if XT_ENABLE_PULSE
#include <xt/pulse/Shared.hpp>
#include <pulse/pulseaudio.h>

PulseDevice::
PulseDevice(bool output): 
_output(output) { }

#endif // XT_ENABLE_PULSE