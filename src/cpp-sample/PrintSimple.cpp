#include <XtCpp.hpp>
#include <memory>
#include <cstdint>
#include <iostream>

int PrintSimpleMain() 
{
  auto audio = Xt::Audio::Init("", nullptr, nullptr);
  for(Xt::System system: Xt::Audio::GetSystems()) 
  {
    std::unique_ptr<Xt::Service> service = Xt::Audio::GetService(system);
    for(int32_t d = 0; d < service->GetDeviceCount(); d++)
      std::cout << system << ": " << *service->OpenDevice(d) << "\n";
  }
  return 0;
}