#include <xt/private/Platform.hpp>
#ifdef __linux__
#include <cassert>

XtPlatform::~XtPlatform() {}
XtPlatform::XtPlatform(void* window): 
XtPlatform() {}

int32_t
XtPlatform::LockIncr(int32_t volatile* dest)
{ return __sync_add_and_fetch(dest, 1); }
int32_t
XtPlatform::LockDecr(int32_t volatile* dest)
{ return __sync_sub_and_fetch(dest, 1); }
int32_t
XtPlatform::Cas(int32_t volatile* dest, int32_t exch, int32_t comp)
{ return __sync_val_compare_and_swap(dest, comp, exch); }

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