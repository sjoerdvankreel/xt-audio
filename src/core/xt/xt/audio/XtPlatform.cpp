#include <xt/audio/XtPlatform.h>
#include <xt/private/OS.hpp>
#include <xt/private/Platform.hpp>
#include <xt/Private.hpp>
#include <cassert>
#include <algorithm>

void XT_CALL 
XtPlatformDestroy(XtPlatform* p)
{
  XT_ASSERT(p != nullptr);
  XT_ASSERT(XtiCalledOnMainThread());
  if(p->ownWindow) XtiDestroyMessageWindow(p->window);
  XtiPlatformDestroy();
  delete p; 
  XtPlatform::instance = nullptr;
}

XtService const* XT_CALL 
XtPlatformGetService(XtPlatform const* p, XtSystem system)
{
  XT_ASSERT(XtiCalledOnMainThread());
  XT_ASSERT(XtSystemALSA <= system && system <= XtSystemDSound);
  for(size_t i = 0; i < p->services.size(); i++)
    if(p->services[i]->GetSystem() == system)
      return p->services[i].get();
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

XtSystem XT_CALL 
XtPlatformSetupToSystem(XtPlatform const* p, XtSetup setup)
{
  XT_ASSERT(XtiCalledOnMainThread());
  XT_ASSERT(XtSetupProAudio <= setup && setup <= XtSetupConsumerAudio);
  switch(setup) 
  {
  case XtSetupProAudio: return XtPlatformGetService(p, XtSystemASIO)? XtSystemASIO: XtSystemJACK;
  case XtSetupSystemAudio: return XtPlatformGetService(p, XtSystemWASAPI)? XtSystemWASAPI: XtSystemALSA;
  case XtSetupConsumerAudio: return XtPlatformGetService(p, XtSystemDSound)? XtSystemDSound: XtSystemPulse;
  default: assert(false); return static_cast<XtSystem>(0);
  }
}