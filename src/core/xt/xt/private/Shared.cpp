#include <xt/private/Shared.hpp>
#include <xt/private/Platform.hpp>
#include <xt/private/Services.hpp>
#include <thread>
#include <cassert>

bool
XtiCalledOnMainThread()
{ 
  auto platform = XtPlatform::instance;
  auto id = std::this_thread::get_id();
  return platform != nullptr && id == platform->threadId;
}

char const* 
XtiGetFaultText(XtSystem system, XtFault fault)
{
  switch(system)
  {
  case XtSystemALSA: return XtiGetAlsaFaultText(fault);
  case XtSystemJACK: return XtiGetJackFaultText(fault);
  case XtSystemASIO: return XtiGetAsioFaultText(fault);
  case XtSystemPulse: return XtiGetPulseFaultText(fault);
  case XtSystemWASAPI: return XtiGetWasapiFaultText(fault);
  case XtSystemDSound: return XtiGetDSoundFaultText(fault);
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
  case XtSystemPulse: return XtiGetPulseFaultCause(fault);
  case XtSystemWASAPI: return XtiGetWasapiFaultCause(fault);
  case XtSystemDSound: return XtiGetDSoundFaultCause(fault);
  default: return assert(false), XtCauseUnknown;
  }
}