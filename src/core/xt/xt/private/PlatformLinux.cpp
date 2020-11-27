#include <xt/private/Platform.hpp>
#ifdef __linux__
#include <cassert>

XtPlatform::~XtPlatform() {}
XtPlatform::XtPlatform(void* window): 
XtPlatform() {}

XtSystem
XtPlatform::SetupToSystem(XtSetup setup)
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