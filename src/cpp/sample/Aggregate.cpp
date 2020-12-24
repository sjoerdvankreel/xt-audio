#include <xt/cpp/XtCpp.hpp>

#include <chrono>
#include <thread>
#include <cstdint>
#include <cstring>
#include <iostream>

static void 
OnXRun(Xt::Stream const& stream, int32_t index, void* user) 
{ std::cout << "XRun on device " << index << ".\n"; }

static void
OnRunning(Xt::Stream const& stream, bool running, uint64_t error, void* user)
{ 
  char const* evt = running? "Started": "Stopped";
  std::cout << "Stream event: " << evt << ", new state: " << stream.IsRunning() << ".\n"; 
  if(error != 0) std::cout << Xt::Audio::GetErrorInfo(error) << ".\n";
}

static uint32_t 
OnBuffer(Xt::Stream const& stream, Xt::Buffer const& buffer, void* user) 
{
  Xt::Format const& format = stream.GetFormat();
  Xt::Attributes attrs = Xt::Audio::GetSampleAttributes(format.mix.sample);
  int32_t bytes = buffer.frames * format.channels.inputs * attrs.size;
  std::memcpy(buffer.output, buffer.input, bytes);
  return 0;
}

int 
AggregateMain()
{
  Xt::Mix mix(48000, Xt::Sample::Int16);
  Xt::Format inputFormat(mix, Xt::Channels(2, 0, 0, 0));
  Xt::Format outputFormat(mix, Xt::Channels(0, 0, 2, 0));
  
  std::unique_ptr<Xt::Platform> platform = Xt::Audio::Init("", nullptr, nullptr);
  Xt::System system = Xt::Audio::SetupToSystem(Xt::Setup::SystemAudio);
  std::unique_ptr<Xt::Service> service = platform->GetService(system);
  if(!service || (service->GetCapabilities() & Xt::ServiceCapsAggregation) == 0) return 0;

  std::optional<std::string> defaultInput = service->GetDefaultDeviceId(false);
  if(!defaultInput.has_value()) return 0;
  std::unique_ptr<Xt::Device> input = service->OpenDevice(defaultInput.value());
  if(!input->SupportsFormat(inputFormat)) return 0;

  std::optional<std::string> defaultOutput = service->GetDefaultDeviceId(true);
  if(!defaultOutput.has_value()) return 0;
  std::unique_ptr<Xt::Device> output = service->OpenDevice(defaultOutput.value());
  if(!output->SupportsFormat(outputFormat)) return 0;

  Xt::AggregateDeviceParams deviceParams[2];
  deviceParams[0] = Xt::AggregateDeviceParams(input.get(), inputFormat.channels, 30.0);
  deviceParams[1] = Xt::AggregateDeviceParams(output.get(), outputFormat.channels, 30.0);
  Xt::StreamParams streamParams(true, OnBuffer, OnXRun, OnRunning);
  Xt::AggregateStreamParams aggregateParams(streamParams, deviceParams, 2, mix, output.get());
  std::unique_ptr<Xt::Stream> stream = service->AggregateStream(aggregateParams, nullptr);
  stream->Start();
  std::this_thread::sleep_for(std::chrono::seconds(2));
  stream->Stop();
  return 0;
}