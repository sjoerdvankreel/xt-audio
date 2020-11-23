#include <xt/private/OS.hpp>
#ifdef __linux__
#include <cassert>

void
XtiPlatformInit() {}
void
XtiPlatformDestroy() {}
void
XtiDestroyMessageWindow(void* window) {}
void* 
XtiCreateMessageWindow()
{ return nullptr; }

XtSystem
XtiSetupToSystem(XtSetup setup)
{
  switch(setup)
  {
  case XtSetupProAudio: return XtSystemJACK;
  case XtSetupSystemAudio: return XtSystemALSA;
  case XtSetupConsumerAudio: return XtSystemPulse;
  default: assert(false); return static_cast<XtSystem>(0);
  }
}

#endif // __linux__