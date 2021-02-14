#include <xt/XtAudio.hpp>

#include <memory>
#include <cstdint>
#include <cstdlib>
#include <iostream>

static void 
OnError(std::string const& message)
{ std::cout << message << std::endl; }

void
PrintDevices(Xt::Service const* service, Xt::DeviceList const* list)
{
  for(int32_t d = 0; d < list->GetCount(); d++)
  {
    std::string id = list->GetId(d);
    try
    {
      std::unique_ptr<Xt::Device> device = service->OpenDevice(id);
      std::optional<Xt::Mix> mix = device->GetMix();
      std::cout << "    Device " << id << ":\n";
      std::cout << "      Name: " << list->GetName(id) << "\n";
      std::cout << "      Capabilities: " << list->GetCapabilities(id) << "\n";
      std::cout << "      Input channels: " << device->GetChannelCount(false) << "\n";
      std::cout << "      Output channels: " << device->GetChannelCount(true) << "\n";
      std::cout << "      Interleaved access: " << device->SupportsAccess(true) << "\n";
      std::cout << "      Non-interleaved access: " << device->SupportsAccess(false) << "\n";
      if(mix) std::cout << "      Current mix: " << mix->rate << " " << mix->sample << "\n";
    } catch(Xt::Exception const& e)
    { std::cout << Xt::Audio::GetErrorInfo(e.GetError()) << "\n"; }
  }
}

int 
PrintDetailedMain()
{
  Xt::Audio::SetOnError(OnError);
  std::unique_ptr<Xt::Platform> platform = Xt::Audio::Init("", nullptr);
  try 
  {
    Xt::Version version = Xt::Audio::GetVersion();
    std::cout << "Version: " << version.major << "." << version.minor << "\n";    
    Xt::System pro = platform->SetupToSystem(Xt::Setup::ProAudio);
    std::cout << "Pro Audio: " << pro << " (" << (platform->GetService(pro) != nullptr) << ")\n";
    Xt::System system = platform->SetupToSystem(Xt::Setup::SystemAudio);
    std::cout << "System Audio: " << system << " (" << (platform->GetService(system) != nullptr) << ")\n";
    Xt::System consumer = platform->SetupToSystem(Xt::Setup::ConsumerAudio);
    std::cout << "Consumer Audio: " << consumer << " (" << (platform->GetService(consumer) != nullptr) << ")\n";

    for(Xt::System s: platform->GetSystems()) 
    {
      std::unique_ptr<Xt::Service> service = platform->GetService(s);
      std::unique_ptr<Xt::DeviceList> all = service->OpenDeviceList(Xt::EnumFlagsAll);
      std::cout << "System " << s << ":\n";
      std::cout << "  Capabilities: " << service->GetCapabilities() << "\n";
      std::optional<std::string> defaultInput = service->GetDefaultDeviceId(false);
      if(defaultInput.has_value())
      {
        std::string name = all->GetName(defaultInput.value());
        std::cout << "  Default input: " << name << " (" << defaultInput.value() << ")\n";
      }
      std::optional<std::string> defaultOutput = service->GetDefaultDeviceId(true);
      if(defaultOutput.has_value())
      {
        std::string name = all->GetName(defaultOutput.value());
        std::cout << "  Default output: " << name << " (" << defaultOutput.value() << ")\n";
      }
      std::unique_ptr<Xt::DeviceList> inputs = service->OpenDeviceList(Xt::EnumFlagsInput);
      std::cout << "  Input device count: " << inputs->GetCount() << "\n";
      PrintDevices(service.get(), inputs.get());
      std::unique_ptr<Xt::DeviceList> outputs = service->OpenDeviceList(Xt::EnumFlagsOutput);
      std::cout << "  Output device count: " << outputs->GetCount() << "\n";
      PrintDevices(service.get(), outputs.get());
    }
    return EXIT_SUCCESS;
  } catch(Xt::Exception const& e) 
  { 
    std::cout << Xt::Audio::GetErrorInfo(e.GetError()) << "\n"; 
    return EXIT_FAILURE;
  } catch(std::exception const& e)
  {
    std::cout << e.what() << "\n"; 
    return EXIT_FAILURE;
  }
}