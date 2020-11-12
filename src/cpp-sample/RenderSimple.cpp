#define _USE_MATH_DEFINES 1
#include <cmath>
#include <chrono>
#include <thread>
#include <climits>
#include <iostream>
#include <XtCpp.hpp>

static float _phase = 0.0f;
static const float Frequency = 440.0f;
static const Xt::Format Format(Xt::Mix(44100, Xt::Sample::Float32), Xt::Channels(0, 0, 1, 0));

static void Render(const Xt::Stream& stream, const Xt::Buffer& buffer, void* user)
{
  float* output = static_cast<float*>(buffer.output);
  for(int f = 0; f < buffer.frames; f++) 
  {
    _phase += Frequency / Format.mix.rate;
    if(_phase >= 1.0f) _phase = -1.0f;
    output[f] = std::sinf(2.0f * _phase * M_PI);
  }
}

int RenderSimpleMain() 
{
  Xt::Audio audio("", nullptr, nullptr);
  Xt::System system = Xt::Audio::SetupToSystem(Xt::Setup::ConsumerAudio);
  std::unique_ptr<Xt::Service> service = Xt::Audio::GetService(system);
  if(!service) return 0;
  std::unique_ptr<Xt::Device> device = service->OpenDefaultDevice(true);
  if(!device || !device->SupportsFormat(Format)) return 0;
  double bufferSize = device->GetBufferSize(Format).current;
  Xt::StreamParams params(Format, true, bufferSize, Render, nullptr);
  std::unique_ptr<Xt::Stream> stream = device->OpenStream(params, nullptr);
  stream->Start();
  std::this_thread::sleep_for(std::chrono::seconds(2));
  stream->Stop();
  return 0;
}