#include <xt-cpp.hpp>
#include <memory>
#include <cstdint>
#include <iostream>

int PrintSimpleMain(int argc, char** argv) {
  Xt::Audio audio("", nullptr, nullptr, nullptr);
  for(auto s: Xt::Audio::GetSystems()) {
    auto service = Xt::Audio::GetService(s);
    for(int32_t d = 0; d < service->GetDeviceCount(); d++)
      std::cout << s << ": " << *service->OpenDevice(d) << "\n";
  }
  return 0;
}