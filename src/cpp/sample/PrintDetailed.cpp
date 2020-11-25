#include <xt/cpp/XtCpp.hpp>

#include <memory>
#include <cstdint>
#include <cstdlib>
#include <iostream>

static void 
OnError(std::string const& location, std::string const& message)
{ std::cout << location << ": " << message << std::endl; }

int 
PrintDetailedMain()
{
  std::unique_ptr<Xt::Platform> platform = Xt::Audio::Init("", nullptr, OnError);
  try 
  {
    Xt::Version version = Xt::Audio::GetVersion();
    std::cout << "Version: " << version.major << "." << version.minor << "\n";    
    Xt::System pro = Xt::Audio::SetupToSystem(Xt::Setup::ProAudio);
    std::cout << "Pro Audio: " << pro << " (" << (platform->GetService(pro) != nullptr) << ")\n";
    Xt::System system = Xt::Audio::SetupToSystem(Xt::Setup::SystemAudio);
    std::cout << "System Audio: " << system << " (" << (platform->GetService(system) != nullptr) << ")\n";
    Xt::System consumer = Xt::Audio::SetupToSystem(Xt::Setup::ConsumerAudio);
    std::cout << "Consumer Audio: " << consumer << " (" << (platform->GetService(consumer) != nullptr) << ")\n";

    for(Xt::System s: platform->GetSystems()) 
    {
      std::unique_ptr<Xt::Service> service = platform->GetService(s);
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
  } catch(Xt::Exception const& e) 
  { 
    std::cout << Xt::Audio::GetErrorInfo(e.GetError()) << "\n"; 
    return EXIT_FAILURE;
  }
}