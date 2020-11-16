#include <xt/cpp/XtCpp.hpp>
#include <memory>
#include <cstdint>
#include <cstdlib>
#include <iostream>

static void OnError(const std::string& location, const std::string& message)
{ std::cout << location << ": " << message << std::endl; }

int PrintDetailedMain()
{
  auto audio = Xt::Audio::Init("Sample", nullptr, OnError);
  try 
  {
    Xt::Version version = Xt::Audio::GetVersion();
    std::cout << "Version: " << version.major << "." << version.minor << "\n";    
    Xt::System pro = Xt::Audio::SetupToSystem(Xt::Setup::ProAudio);
    std::cout << "Pro Audio: " << pro << " (" << (Xt::Audio::GetService(pro) != nullptr) << ")\n";
    Xt::System system = Xt::Audio::SetupToSystem(Xt::Setup::SystemAudio);
    std::cout << "System Audio: " << system << " (" << (Xt::Audio::GetService(system) != nullptr) << ")\n";
    Xt::System consumer = Xt::Audio::SetupToSystem(Xt::Setup::ConsumerAudio);
    std::cout << "Consumer Audio: " << consumer << " (" << (Xt::Audio::GetService(consumer) != nullptr) << ")\n";

    for(Xt::System s: Xt::Audio::GetSystems()) 
    {
      std::unique_ptr<Xt::Service> service = Xt::Audio::GetService(s);
      std::cout << "System " << s << ":\n";
      std::cout << "  Device count: " << service->GetDeviceCount() << "\n";
      std::cout << "  Capabilities: " << service->GetCapabilities() << "\n";
      std::unique_ptr<Xt::Device> defaultInput = service->OpenDefaultDevice(false);
      if(defaultInput) std::cout << "  Default input: " << *defaultInput << "\n";
      std::unique_ptr<Xt::Device> defaultOutput = service->OpenDefaultDevice(true);
      if(defaultOutput) std::cout << "  Default output: " << *defaultOutput << "\n";

      for(int32_t d = 0; d < service->GetDeviceCount(); d++)
      {
        std::unique_ptr<Xt::Device> device = service->OpenDevice(d);
        std::optional<Xt::Mix> mix = device->GetMix();
        std::cout << "  Device " << *device << ":" << "\n";
        std::cout << "    Input channels: " << device->GetChannelCount(false) << "\n";
        std::cout << "    Output channels: " << device->GetChannelCount(true) << "\n";
        std::cout << "    Interleaved access: " << device->SupportsAccess(true) << "\n";
        std::cout << "    Non-interleaved access: " << device->SupportsAccess(false) << "\n";
        if(mix) std::cout << "    Current mix: " << mix->rate << " " << mix->sample << "\n";
      }
    }
    return EXIT_SUCCESS;
  }
  catch(const Xt::Exception& e) 
  { 
    std::cout << Xt::Audio::GetErrorInfo(e.GetError()) << "\n"; 
    return EXIT_FAILURE;
  }
}