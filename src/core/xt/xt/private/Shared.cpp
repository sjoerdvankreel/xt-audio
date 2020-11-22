#include <xt/private/Shared.hpp>
#include <xt/alsa/Fault.hpp>
#include <xt/asio/Fault.hpp>
#include <xt/jack/Fault.hpp>
#include <xt/pulse/Fault.hpp>
#include <xt/wasapi/Fault.hpp>
#include <xt/dsound/Fault.hpp>
#include <cassert>

char const* 
XtiGetFaultText(XtSystem system, XtFault fault)
{
  switch(system)
  {
  case XtSystemALSA: return XtiGetAlsaFaultText(fault);
  case XtSystemJACK: return XtiGetJackFaultText(fault);
  case XtSystemASIO: return XtiGetAsioFaultText(fault);
  case XtSystemWASAPI: return XtiGetWasapiFaultText(fault);
  case XtSystemDSound: return XtiGetDSoundFaultText(fault);
  case XtSystemPulseAudio: return XtiGetPulseAudioFaultText(fault);
  default: return assert(false), nullptr;
  }
}

XtCause 
XtiGetFaultCause(XtSystem system, XtFault fault)
{
  switch(system)
  {
  case XtSystemALSA: return XtiGetAlsaFaultCause(fault);
  case XtSystemJACK: return XtiGetJackFaultCause(fault);
  case XtSystemASIO: return XtiGetAsioFaultCause(fault);
  case XtSystemWASAPI: return XtiGetWasapiFaultCause(fault);
  case XtSystemDSound: return XtiGetDSoundFaultCause(fault);
  case XtSystemPulseAudio: return XtiGetPulseAudioFaultCause(fault);
  default: return assert(false), XtCauseUnknown;
  }
}