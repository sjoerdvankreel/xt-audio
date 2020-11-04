#include <XtCpp.hpp>
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif
#include <climits>
#include <cmath>
#include <iostream>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

static double phase = 0.0;
static const double Frequency = 440.0;
static const Xt::Format Format(Xt::Mix(44100, Xt::Sample::Float32), Xt::Channels(0, 0, 1, 0));

static void Render(
  const Xt::Stream& stream, const Xt::Buffer& buffer, const Xt::Time& time, uint64_t error, void* user) {

  for(int f = 0; f < buffer.frames; f++) {
    phase += Frequency / Format.mix.rate;
    if(phase >= 1.0)
      phase = -1.0;
    ((float*)buffer.output)[f] = (float)sin(2.0 * phase * M_PI);
  }
}

int RenderSimpleMain(int argc, char** argv) {

  Xt::Audio audio("", nullptr, nullptr);
  auto system = Xt::Audio::SetupToSystem(Xt::Setup::ConsumerAudio);
  std::unique_ptr<Xt::Service> service = Xt::Audio::GetService(system);
  if(!service)
    return 0;

  std::unique_ptr<Xt::Device> device = service->OpenDefaultDevice(true);
  if(!device || !device->SupportsFormat(Format)) 
    return 0;

  Xt::BufferSize size = device->GetBufferSize(Format);
  std::unique_ptr<Xt::Stream> stream = device->OpenStream(Format, true, size.current, Render, nullptr, nullptr);
  stream->Start();
#if _WIN32
  Sleep(1000);
#else
  usleep(1000 * 1000);
#endif
  stream->Stop();
  return 0;
}