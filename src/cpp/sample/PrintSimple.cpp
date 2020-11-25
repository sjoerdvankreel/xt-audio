#include <xt/cpp/XtCpp.hpp>

#include <memory>
#include <cstdint>
#include <iostream>

int 
PrintSimpleMain() 
{
  std::unique_ptr<Xt::Platform> platform = Xt::Audio::Init("", nullptr, nullptr);
  for(Xt::System system: platform->GetSystems()) 
  {
    std::unique_ptr<Xt::Service> service = platform->GetService(system);
    for(int32_t d = 0; d < service->GetDeviceCount(); d++)
      std::cout << system << ": " << *service->OpenDevice(d) << "\n";
  }
  return 0;
}