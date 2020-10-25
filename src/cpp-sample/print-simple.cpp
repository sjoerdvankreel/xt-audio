#include <xt-cpp.hpp>
#include <memory>
#include <iostream>

int PrintSimpleMain(int argc, char** argv) {
  Xt::Audio audio("", nullptr, nullptr, nullptr);
  for(int s = 0; s < Xt::Audio::GetServiceCount(); s++) {
    auto service = Xt::Audio::GetServiceByIndex(s);
    for(int d = 0; d < service->GetDeviceCount(); d++)
      std::cout << *service << ": " << *service->OpenDevice(d) << "\n";
  }
  return 0;
}