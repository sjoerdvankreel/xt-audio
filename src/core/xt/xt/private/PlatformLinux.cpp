#ifdef __linux__
#include <xt/private/Platform.hpp>
#include <errno.h>
#include <pthread.h>

XtPlatform::~XtPlatform() { }
void XtPlatform::EndThread() { }
void XtPlatform::BeginThread() { }
bool XtPlatform::Init(void* window) { return true; }

XtSystem
XtPlatform::SetupToSystem(XtSetup setup) const
{
  switch(setup)
  {
  case XtSetupProAudio: return XtSystemJACK;
  case XtSetupSystemAudio: return XtSystemALSA;
  case XtSetupConsumerAudio: return XtSystemPulse;
  default: XT_ASSERT(false); return static_cast<XtSystem>(0);
  }
}

void 
XtPlatform::RevertThreadPriority(int32_t policy, int32_t previous)
{ 
  struct sched_param param;
  param.sched_priority = previous;
  XT_ASSERT(pthread_setschedparam(pthread_self(), policy, &param) == 0);
}

void 
XtPlatform::RaiseThreadPriority(int32_t* policy, int32_t* previous)
{ 
  int32_t maxPriority;
  struct sched_param param;
  XT_ASSERT(pthread_getschedparam(pthread_self(), policy, &param) == 0);
  *previous = param.sched_priority;
  param.sched_priority = sched_get_priority_max(*policy);
  XT_ASSERT(pthread_setschedparam(pthread_self(), *policy, &param) == 0);
}

#endif // __linux__