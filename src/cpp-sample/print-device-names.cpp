#include <xt-cpp.hpp>
#include <iostream>

int PrintDevicesMain(int argc, char** argv) {

  Xt::Audio init("", nullptr, nullptr, nullptr);
  for (int32_t s = 0; s < Xt::Audio::GetServiceCount(); s++) {
    std::unique_ptr<Xt::Service> service = Xt::Audio::GetServiceByIndex(s);
    for (int32_t d = 0; d < service->GetDeviceCount(); d++) {
      std::unique_ptr<Xt::Device> device = service->OpenDevice(d);
      std::cout << service->GetName() << ": " << device->GetName() << "\n";
    }
  }
  return 0;
}
