#include <xt/api/XtPlatform.h>
#include <xt/private/Platform.hpp>

void XT_CALL
XtPlatformDestroy(XtPlatform* p)
{
  XT_ASSERT_VOID_API(XtiCalledOnMainThread());
  delete p; 
  XtPlatform::instance = nullptr;
}

XtService const* XT_CALL 
XtPlatformGetService(XtPlatform const* p, XtSystem system)
{
  XT_ASSERT_API(p != nullptr);
  XT_ASSERT_API(XtiCalledOnMainThread());
  XT_ASSERT_API(XtSystemALSA <= system && system <= XtSystemDSound);
  return p->GetService(system);
}

XtSystem XT_CALL 
XtPlatformSetupToSystem(XtPlatform const* p, XtSetup setup)
{
  XT_ASSERT_API(p != nullptr);
  XT_ASSERT_API(XtiCalledOnMainThread());
  XT_ASSERT_API(XtSetupProAudio <= setup && setup <= XtSetupConsumerAudio);
  return p->SetupToSystem(setup);
}

void XT_CALL 
XtPlatformGetSystems(XtPlatform const* p, XtSystem* buffer, int32_t* size)
{
  XT_ASSERT_VOID_API(p != nullptr);
  XT_ASSERT_VOID_API(XtiCalledOnMainThread());
  return p->GetSystems(buffer, size);
}