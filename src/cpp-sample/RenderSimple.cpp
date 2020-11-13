#define _USE_MATH_DEFINES 1
#include <XtCpp.hpp>
#include <cmath>
#include <chrono>
#include <thread>

static float _phase = 0.0f;
static const float Frequency = 440.0f;
static const Xt::Channels Channels(0, 0, 1, 0);
static const Xt::Mix Mix(44100, Xt::Sample::Float32);
static const Xt::Format Format(Mix, Channels);

static float NextSample()
{
  _phase += Frequency / Mix.rate;
  if (_phase >= 1.0f) _phase = -1.0f;
  return std::sinf(2.0f * _phase * static_cast<float>(M_PI));
}

static void Render(const Xt::Stream& stream, const Xt::Buffer& buffer, void* user)
{
  float* output = static_cast<float*>(buffer.output);
  for (int f = 0; f < buffer.frames; f++) output[f] = NextSample();
}

int RenderSimpleMain()
{
  Xt::Audio audio("", nullptr, nullptr);
  Xt::System system = Xt::Audio::SetupToSystem(Xt::Setup::ConsumerAudio);
  std::unique_ptr<Xt::Service> service = Xt::Audio::GetService(system);
  if (!service) return 0;

  std::unique_ptr<Xt::Device> device = service->OpenDefaultDevice(true);
  if (!device || !device->SupportsFormat(Format)) return 0;

  double bufferSize = device->GetBufferSize(Format).current;
  Xt::StreamParams streamParams(true, Render, nullptr);
  Xt::DeviceStreamParams deviceParams(Format, bufferSize, streamParams);
  std::unique_ptr<Xt::Stream> stream = device->OpenStream(deviceParams, nullptr);
  stream->Start();
  std::this_thread::sleep_for(std::chrono::seconds(2));
  stream->Stop();

  return 0;
}