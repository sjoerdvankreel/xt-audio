#include <xt-cpp.hpp>
#include <iostream>

static void OnFatal() {
  std::cout << "Fatal error.\n";
}

static void OnTrace(Xt::Level level, const std::string& message) {
  if (level != Xt::Level::Info)
    std::cout << "-- " << level << ": " << message << "\n";
}

int PrintInfoMain(int argc, char** argv) {

  try {
    Xt::Audio init("Sample", nullptr, &OnTrace, &OnFatal);
    std::cout << "Win32: " << Xt::Audio::IsWin32() << "\n";
    std::cout << "Version: " << Xt::Audio::GetVersion() << "\n";
    std::cout << "Pro Audio: " << Xt::Audio::GetServiceBySetup(Xt::Setup::ProAudio)->GetName() << "\n";
    std::cout << "System Audio: " << Xt::Audio::GetServiceBySetup(Xt::Setup::SystemAudio)->GetName() << "\n";
    std::cout << "Consumer Audio: " << Xt::Audio::GetServiceBySetup(Xt::Setup::ConsumerAudio)->GetName() << "\n";

    for (int32_t s = 0; s < Xt::Audio::GetServiceCount(); s++) {
      std::unique_ptr<Xt::Service> service = Xt::Audio::GetServiceByIndex(s);
      std::cout << "Service " << service->GetName() << ":\n";
      std::cout << "  System: " << service->GetSystem() << "\n";
      std::cout << "  Device count: " << service->GetDeviceCount() << "\n";
      std::cout << "  Capabilities: " << service->GetCapabilities() << "\n";
      std::unique_ptr<Xt::Device> defaultInput = service->OpenDefaultDevice(false);
      if(defaultInput)
        std::cout << "  Default input: " << *defaultInput << "\n";
      std::unique_ptr<Xt::Device> defaultOutput = service->OpenDefaultDevice(true);
      if(defaultOutput)
        std::cout << "  Default output: " << *defaultOutput << "\n";

      for (int d = 0; d < service->GetDeviceCount(); d++) {
        std::unique_ptr<Xt::Device> device = service->OpenDevice(d);
        std::cout << "  Device " << device->GetName() << ":\n";
        std::cout << "    System: " << device->GetSystem() << "\n";
        std::unique_ptr<Xt::Mix> mix = device->GetMix();
        if(mix)
          std::cout << "    Current mix: " << *mix << "\n";
        else
          std::cout << "    Current mix: null\n";
        std::cout << "    Input channels: " << device->GetChannelCount(false) << "\n";
        std::cout << "    Output channels: " << device->GetChannelCount(true) << "\n";
        std::cout << "    Interleaved access: " << device->SupportsAccess(true) << "\n";
        std::cout << "    Non-interleaved access: " << device->SupportsAccess(false) << "\n";
      }
    }
  } catch(const Xt::Exception& e) {
    std::cout << "Error: " 
              << "system " << Xt::Exception::GetSystem(e.GetError()) << ", "
              << "fault " << Xt::Exception::GetFault(e.GetError()) << ", "
              << "cause " << Xt::Exception::GetCause(e.GetError()) << ", "
              << "text " << Xt::Exception::GetText(e.GetError()) << ", "
              << "message " << e << ".\n";
  }
  return 0;
}
