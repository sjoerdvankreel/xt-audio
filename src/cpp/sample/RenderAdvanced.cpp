#define _USE_MATH_DEFINES 1
#include <xt/XtAudio.hpp>

#include <cmath>
#include <chrono>
#include <thread>
#include <cstdint>
#include <iostream>

static float _phase = 0.0f;
static float const Frequency = 440.0f;
static Xt::Mix const Mix(44100, Xt::Sample::Float32);

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

static void 
RunStream(Xt::Stream* stream)
{
  stream->Start();
  std::this_thread::sleep_for(std::chrono::seconds(2));
  stream->Stop();
}

static float 
NextSample()
{
  _phase += Frequency / Mix.rate;
  if (_phase >= 1.0f) _phase = -1.0f;
  return sinf(2.0f * _phase * static_cast<float>(M_PI));
}

static uint32_t 
OnInterleavedBuffer(Xt::Stream const& stream, Xt::Buffer const& buffer, void* user)
{
  float* output = static_cast<float*>(buffer.output);
  int32_t channels = stream.GetFormat().channels.outputs;
  int32_t size = Xt::Audio::GetSampleAttributes(Mix.sample).size;
  for(int32_t f = 0; f < buffer.frames; f++) 
  {
    float sample = NextSample();
    for (int32_t c = 0; c < channels; c++) output[f * channels + c] = sample;
  }
  return 0;
}

static uint32_t 
OnNonInterleavedBuffer(Xt::Stream const& stream, Xt::Buffer const& buffer, void* user) 
{
  float** output = static_cast<float**>(buffer.output);
  int32_t channels = stream.GetFormat().channels.outputs;
  int32_t size = Xt::Audio::GetSampleAttributes(Mix.sample).size;
  for(int32_t f = 0; f < buffer.frames; f++) 
  {
    float sample = NextSample();
    for(int32_t c = 0; c < channels; c++) output[c][f] = sample;
  }
  return 0;
}

int 
RenderAdvancedMain() 
{
  std::unique_ptr<Xt::Platform> platform = Xt::Audio::Init("", nullptr, nullptr);
  Xt::Format format(Mix, Xt::Channels(0, 0, 2, 0));
  Xt::System system = Xt::Audio::SetupToSystem(Xt::Setup::ConsumerAudio);
  std::unique_ptr<Xt::Service> service = platform->GetService(system);
  if(!service) return 0;

  std::optional<std::string> id = service->GetDefaultDeviceId(true);
  if(!id.has_value()) return 0;
  std::unique_ptr<Xt::Device> device = service->OpenDevice(id.value());
  if (!device->SupportsFormat(format)) return 0;
  Xt::BufferSize size = device->GetBufferSize(format);

  std::cout << "Render interleaved...\n";
  Xt::StreamParams streamParams(true, OnInterleavedBuffer, OnXRun, OnRunning);
  Xt::DeviceStreamParams deviceParams(streamParams, format, size.current);
  {
    std::unique_ptr<Xt::Stream> stream = device->OpenStream(deviceParams, nullptr);
    RunStream(stream.get());
  }

  std::cout << "Render non-interleaved...\n";
  streamParams = Xt::StreamParams(false, OnNonInterleavedBuffer, OnXRun, OnRunning);
  deviceParams = Xt::DeviceStreamParams(streamParams, format, size.current);
  {
    std::unique_ptr<Xt::Stream> stream = device->OpenStream(deviceParams, nullptr);
    RunStream(stream.get());
  }

  std::cout << "Render interleaved (channel 0)...\n";
  Xt::Format sendTo0(Mix, Xt::Channels(0, 0, 1, 1ULL << 0));
  streamParams = Xt::StreamParams(true, OnInterleavedBuffer, OnXRun, OnRunning);
  deviceParams = Xt::DeviceStreamParams(streamParams, sendTo0, size.current);
  {
    std::unique_ptr<Xt::Stream> stream = device->OpenStream(deviceParams, nullptr);
    RunStream(stream.get());
  }

  std::cout << "Render non-interleaved (channel 1)...\n";
  Xt::Format sendTo1(Mix, Xt::Channels(0, 0, 1, 1ULL << 1));
  streamParams = Xt::StreamParams(false, OnNonInterleavedBuffer, OnXRun, OnRunning);
  deviceParams = Xt::DeviceStreamParams(streamParams, sendTo1, size.current);
  {
    std::unique_ptr<Xt::Stream> stream = device->OpenStream(deviceParams, nullptr);
    RunStream(stream.get());
  }

  return 0;
}