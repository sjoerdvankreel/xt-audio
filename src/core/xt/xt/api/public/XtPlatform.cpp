#include <xt/api/public/XtPlatform.h>
#include <xt/api/private/Platform.hpp>

void XT_CALL
XtPlatformDestroy(XtPlatform* p)
{
  XT_ASSERT(p != nullptr);
  XT_ASSERT(XtiCalledOnMainThread());
  delete p; 
  XtPlatform::instance = nullptr;
}

XtService const* XT_CALL 
XtPlatformGetService(XtPlatform const* p, XtSystem system)
{
  XT_ASSERT(XtiCalledOnMainThread());
  XT_ASSERT(XtSystemALSA <= system && system <= XtSystemDSound);
  return p->GetService(system);
}

void XT_CALL 
XtPlatformGetSystems(XtPlatform const* p, XtSystem* buffer, int32_t* size)
{
  XT_ASSERT(XtiCalledOnMainThread());
  return p->GetSystems(buffer, size);
}