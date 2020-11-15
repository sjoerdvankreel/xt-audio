#include <XtCpp.hpp>
#include <chrono>
#include <thread>
#include <cstdint>
#include <cstring>
#include <iostream>

static void OnXRun(int32_t index, void* user) 
{ std::cout << "XRun on device " << index << ".\n"; }

static void OnBuffer(const Xt::Stream& stream, const Xt::Buffer& buffer, void* user) 
{
  const Xt::Format& format = stream.GetFormat();
  Xt::Attributes attrs = Xt::Audio::GetSampleAttributes(format.mix.sample);
  int32_t bytes = buffer.frames * format.channels.inputs * attrs.size;
  std::memcpy(buffer.output, buffer.input, bytes);
}

int AggregateMain()
{
  Xt::Mix mix(48000, Xt::Sample::Int16);
  Xt::Format inputFormat(mix, Xt::Channels(2, 0, 0, 0));
  Xt::Format outputFormat(mix, Xt::Channels(0, 0, 2, 0));
  
  Xt::Audio audio("", nullptr, nullptr);
  Xt::System system = Xt::Audio::SetupToSystem(Xt::Setup::SystemAudio);
  std::unique_ptr<Xt::Service> service = Xt::Audio::GetService(system);
  if(!service) return 0;

  std::unique_ptr<Xt::Device> input = service->OpenDefaultDevice(false);
  if(!input || !input->SupportsFormat(inputFormat)) return 0;
  std::unique_ptr<Xt::Device> output = service->OpenDefaultDevice(true);
  if(!output || !output->SupportsFormat(outputFormat)) return 0;

  Xt::AggregateDeviceParams deviceParams[2];
  deviceParams[0] = Xt::AggregateDeviceParams(input.get(), inputFormat.channels, 30.0);
  deviceParams[1] = Xt::AggregateDeviceParams(output.get(), outputFormat.channels, 30.0);
  Xt::StreamParams streamParams(true, OnBuffer, OnXRun);
  Xt::AggregateStreamParams aggregateParams(streamParams, deviceParams, 2, mix, output.get());
  std::unique_ptr<Xt::Stream> stream = service->AggregateStream(aggregateParams, nullptr);
  stream->Start();
  std::this_thread::sleep_for(std::chrono::seconds(2));
  stream->Stop();
  return 0;
}