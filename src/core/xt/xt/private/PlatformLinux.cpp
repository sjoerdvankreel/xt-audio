#include <xt/private/Platform.hpp>
#ifdef __linux__
#include <xt/Private.hpp>

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
  default: XT_ASSERT(false); return static_cast<XtSystem>(0);
  }
}

#endif // __linux__