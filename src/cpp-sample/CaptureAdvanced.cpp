#include <xt/cpp/XtCpp.hpp>

#include <chrono>
#include <thread>
#include <fstream>
#include <iostream>

static const Xt::Channels Channels(2, 0, 0, 0);
static const Xt::Mix Mix(44100, Xt::Sample::Int24);
static const Xt::Format Format(Mix, Channels);

static void OnXRun(int32_t index, void* user) 
{ std::cout << "XRun on device " << index << ".\n"; }

static void RunStream(Xt::Stream* stream)
{
  stream->Start();
  std::this_thread::sleep_for(std::chrono::seconds(2));
  stream->Stop();
}

static int32_t GetBufferSize(int32_t channels, int32_t frames)
{
  int32_t size = Xt::Audio::GetSampleAttributes(Mix.sample).size;
  return channels * frames * size;
}

static void OnInterleavedBuffer(const Xt::Stream& stream, const Xt::Buffer& buffer, void* user) 
{
  auto output = static_cast<std::ofstream*>(user);
  auto input = static_cast<const char*>(buffer.input);
  int32_t bytes = GetBufferSize(Channels.inputs, buffer.frames);
  output->write(input, bytes);
}

static void OnNonInterleavedBuffer(const Xt::Stream& stream, const Xt::Buffer& buffer, void* user) 
{
  auto output = static_cast<std::ofstream*>(user);  
  auto input = static_cast<const char* const*>(buffer.input);
  int32_t size = Xt::Audio::GetSampleAttributes(Mix.sample).size;
  for(int32_t f = 0; f < buffer.frames; f++)
    for(int32_t c = 0; c < Channels.inputs; c++)
      output->write(&input[c][f * size], size);
}

int CaptureAdvancedMain() 
{
  auto audio = Xt::Audio::Init("", nullptr, nullptr);
  Xt::System system = Xt::Audio::SetupToSystem(Xt::Setup::ConsumerAudio);
  std::unique_ptr<Xt::Service> service = Xt::Audio::GetService(system);
  if(!service)return 0; 

  std::unique_ptr<Xt::Device> device = service->OpenDefaultDevice(false);
  if(!device || !device->SupportsFormat(Format)) return 0;  
  Xt::BufferSize size = device->GetBufferSize(Format);

  std::cout << "Capture interleaved...\n";
  Xt::StreamParams streamParams(true, OnInterleavedBuffer, OnXRun);
  Xt::DeviceStreamParams deviceParams(streamParams, Format, size.current);
  std::ofstream interleaved("xt-audio-interleaved.raw", std::ios::out | std::ios::binary);
  std::unique_ptr<Xt::Stream> stream = device->OpenStream(deviceParams, &interleaved);
  RunStream(stream.get());

  std::cout << "Capture non-interleaved...\n";
  streamParams = Xt::StreamParams(false, OnNonInterleavedBuffer, OnXRun);
  deviceParams = Xt::DeviceStreamParams(streamParams, Format, size.current);
  std::ofstream nonInterleaved("xt-audio-non-interleaved.raw", std::ios::out | std::ios::binary);
  stream = device->OpenStream(deviceParams, &nonInterleaved);
  RunStream(stream.get());

  return 0;
}