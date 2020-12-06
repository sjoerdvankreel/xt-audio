#include <xt/cpp/XtCpp.hpp>

#include <memory>
#include <cstdint>
#include <cstdlib>
#include <iostream>

static void 
OnError(Xt::Location const& location, std::string const& message)
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
      std::unique_ptr<Xt::DeviceList> list = service->OpenDeviceList();
      std::cout << "System " << s << ":\n";
      std::cout << "  Device count: " << list->GetCount() << "\n";
      std::cout << "  Capabilities: " << service->GetCapabilities() << "\n";

      int32_t defaultInput = list->GetDefault(false);
      if(defaultInput != -1)
      {
        std::string id = list->GetId(defaultInput);
        std::string name = list->GetName(defaultInput);
        std::cout << "  Default input: " << name << " (" << id << ")\n";
      }
      int32_t defaultOutput = list->GetDefault(true);
      if(defaultOutput != -1)
      {
        std::string id = list->GetId(defaultOutput);
        std::string name = list->GetName(defaultOutput);
        std::cout << "  Default output: " << name << " (" << id << ")\n";
      }

      for(int32_t d = 0; d < list->GetCount(); d++)
      {
        std::string id = list->GetId(d);
        std::unique_ptr<Xt::Device> device = service->OpenDevice(id);
        try
        {
          std::optional<Xt::Mix> mix = device->GetMix();
          std::cout << "  Device " << id << "\n";
          std::cout << "    Name: " << list->GetName(d) << "\n";
          std::cout << "    Input channels: " << device->GetChannelCount(false) << "\n";
          std::cout << "    Output channels: " << device->GetChannelCount(true) << "\n";
          std::cout << "    Interleaved access: " << device->SupportsAccess(true) << "\n";
          std::cout << "    Non-interleaved access: " << device->SupportsAccess(false) << "\n";
          if(mix) std::cout << "    Current mix: " << mix->rate << " " << mix->sample << "\n";
        } catch(Xt::Exception const& e)
        { std::cout << Xt::Audio::GetErrorInfo(e.GetError()) << "\n"; }
      }
    }
    return EXIT_SUCCESS;
  } catch(Xt::Exception const& e) 
  { 
    std::cout << Xt::Audio::GetErrorInfo(e.GetError()) << "\n"; 
    return EXIT_FAILURE;
  }
}