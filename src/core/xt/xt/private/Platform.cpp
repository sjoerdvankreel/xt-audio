#include <xt/private/Platform.hpp>
#include <vector>
#include <cstring>
#include <algorithm>

XtService const*
XtPlatform::GetService(XtSystem system) const
{
  for(size_t i = 0; i < _services.size(); i++)
    if(_services[i]->GetSystem() == system)
      return _services[i].get();
  return nullptr;
}

void
XtPlatform::GetSystems(XtSystem* buffer, int32_t* size) const
{
  std::vector<XtSystem> systems;
  if(GetService(XtSystemASIO) != nullptr) systems.push_back(XtSystemASIO);
  if(GetService(XtSystemJACK) != nullptr) systems.push_back(XtSystemJACK);
  if(GetService(XtSystemALSA) != nullptr) systems.push_back(XtSystemALSA);
  if(GetService(XtSystemPulse) != nullptr) systems.push_back(XtSystemPulse);
  if(GetService(XtSystemDSound) != nullptr) systems.push_back(XtSystemDSound);
  if(GetService(XtSystemWASAPI) != nullptr) systems.push_back(XtSystemWASAPI);
  auto count = static_cast<int32_t>(systems.size());
  if(buffer == nullptr) *size = count;
  else memcpy(buffer, systems.data(), std::min(*size, count)*sizeof(XtSystem));
}