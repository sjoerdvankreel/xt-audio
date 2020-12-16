#include <xt/cpp/XtCpp.hpp>

#include <chrono>
#include <thread>
#include <fstream>
#include <iostream>

static Xt::Channels const Channels(2, 0, 0, 0);
static Xt::Mix const Mix(44100, Xt::Sample::Int24);
static Xt::Format const Format(Mix, Channels);

static void 
OnXRun(int32_t index, void* user) 
{ std::cout << "XRun on device " << index << ".\n"; }

static void
OnRunning(Xt::Stream const& stream, bool running, void* user)
{ 
  char const* evt = running? "Started": "Stopped";
  std::cout << "Stream event: " << evt << ", new state: " << stream.IsRunning() << "\n"; 
}

static void 
RunStream(Xt::Stream* stream)
{
  stream->Start();
  std::this_thread::sleep_for(std::chrono::seconds(2));
  stream->Stop();
}

static int32_t 
GetBufferSize(int32_t channels, int32_t frames)
{
  int32_t size = Xt::Audio::GetSampleAttributes(Mix.sample).size;
  return channels * frames * size;
}

static void 
OnInterleavedBuffer(Xt::Stream const& stream, Xt::Buffer const& buffer, void* user) 
{
  auto output = static_cast<std::ofstream*>(user);
  auto input = static_cast<char const*>(buffer.input);
  int32_t bytes = GetBufferSize(Channels.inputs, buffer.frames);
  output->write(input, bytes);
}

static void 
OnNonInterleavedBuffer(Xt::Stream const& stream, Xt::Buffer const& buffer, void* user) 
{
  auto output = static_cast<std::ofstream*>(user);  
  auto input = static_cast<char const* const*>(buffer.input);
  int32_t size = Xt::Audio::GetSampleAttributes(Mix.sample).size;
  for(int32_t f = 0; f < buffer.frames; f++)
    for(int32_t c = 0; c < Channels.inputs; c++)
      output->write(&input[c][f * size], size);
}

int 
CaptureAdvancedMain() 
{
  std::unique_ptr<Xt::Platform> platform = Xt::Audio::Init("", nullptr, nullptr);
  Xt::System system = Xt::Audio::SetupToSystem(Xt::Setup::ConsumerAudio);
  std::unique_ptr<Xt::Service> service = platform->GetService(system);
  if(!service) return 0; 

  std::optional<std::string> id = service->GetDefaultDeviceId(false);
  if(!id.has_value()) return 0;
  std::unique_ptr<Xt::Device> device = service->OpenDevice(id.value());
  if(!device->SupportsFormat(Format)) return 0;
  Xt::BufferSize size = device->GetBufferSize(Format);

  std::cout << "Capture interleaved...\n";
  Xt::StreamParams streamParams(true, OnInterleavedBuffer, OnXRun, OnRunning);
  Xt::DeviceStreamParams deviceParams(streamParams, Format, size.current);
  std::ofstream interleaved("xt-audio-interleaved.raw", std::ios::out | std::ios::binary);
  std::unique_ptr<Xt::Stream> stream = device->OpenStream(deviceParams, &interleaved);
  RunStream(stream.get());

  std::cout << "Capture non-interleaved...\n";
  streamParams = Xt::StreamParams(false, OnNonInterleavedBuffer, OnXRun, OnRunning);
  deviceParams = Xt::DeviceStreamParams(streamParams, Format, size.current);
  std::ofstream nonInterleaved("xt-audio-non-interleaved.raw", std::ios::out | std::ios::binary);
  stream = device->OpenStream(deviceParams, &nonInterleaved);
  RunStream(stream.get());

  return 0;
}