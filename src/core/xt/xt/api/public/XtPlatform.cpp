#include <xt/api/public/XtPlatform.h>
#include <xt/api/private/Platform.hpp>
#include <cstring>
#include <algorithm>

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
  for(size_t i = 0; i < p->_services.size(); i++)
    if(p->_services[i]->GetSystem() == system)
      return p->_services[i].get();
  return nullptr;
}

void XT_CALL 
XtPlatformGetSystems(XtPlatform const* p, XtSystem* buffer, int32_t* size)
{
  XT_ASSERT(XtiCalledOnMainThread());
  std::vector<XtSystem> systems;
  if(XtPlatformGetService(p, XtSystemASIO) != nullptr) systems.push_back(XtSystemASIO);
  if(XtPlatformGetService(p, XtSystemJACK) != nullptr) systems.push_back(XtSystemJACK);
  if(XtPlatformGetService(p, XtSystemALSA) != nullptr) systems.push_back(XtSystemALSA);
  if(XtPlatformGetService(p, XtSystemPulse) != nullptr) systems.push_back(XtSystemPulse);
  if(XtPlatformGetService(p, XtSystemDSound) != nullptr) systems.push_back(XtSystemDSound);
  if(XtPlatformGetService(p, XtSystemWASAPI) != nullptr) systems.push_back(XtSystemWASAPI);
  auto count = static_cast<int32_t>(systems.size());
  if(buffer == nullptr) *size = count;
  else memcpy(buffer, systems.data(), std::min(*size, count)*sizeof(XtSystem));
}