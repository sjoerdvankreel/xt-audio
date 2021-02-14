#include <xt/XtAudio.hpp>

#include <memory>
#include <cstdint>
#include <iostream>

int 
PrintSimpleMain() 
{
  std::unique_ptr<Xt::Platform> platform = Xt::Audio::Init("", nullptr);
  for(Xt::System system: platform->GetSystems()) 
  {
    std::unique_ptr<Xt::Service> service = platform->GetService(system);
    std::unique_ptr<Xt::DeviceList> list = service->OpenDeviceList(Xt::EnumFlagsAll);
    for(int32_t d = 0; d < list->GetCount(); d++)
    {
      std::string id = list->GetId(d);
      std::cout << system << ": " << list->GetName(id) << "\n";
    }
  }
  return 0;
}