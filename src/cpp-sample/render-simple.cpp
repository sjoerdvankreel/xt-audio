#include <xt-cpp.hpp>
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
static const double Frequency = 660.0;
static const Xt::Format Format(Xt::Mix(44100, Xt::Sample::Float32), 0, 0, 1, 0);

static void Render(
  const Xt::Stream& stream, const void* input, void* output, int32_t frames,
  double time, uint64_t position, bool timeValid, uint64_t error, void* user) {

  for(int f = 0; f < frames; f++) {
    phase += Frequency / Format.mix.rate;
    if(phase >= 1.0)
      phase = -1.0;
    ((float*)output)[f] = (float)sin(phase * M_PI);
  }
}

int RenderSimpleMain(int argc, char** argv) {

  Xt::Audio audio("", nullptr, nullptr, nullptr);
  std::unique_ptr< Xt::Service> service = Xt::Audio::GetServiceBySetup(Xt::Setup::ConsumerAudio);
  std::unique_ptr<Xt::Device> device = service->OpenDefaultDevice(true);
  if(device  && device->SupportsFormat(Format)) {

    Xt::Buffer buffer = device->GetBuffer(Format);
    std::unique_ptr<Xt::Stream> stream = device->OpenStream(Format, true, buffer.current, Render, nullptr);
    stream->Start();
#if _WIN32
    Sleep(1000);
#else
    usleep(1000 * 1000);
#endif
    stream->Stop();
  }
  return 0;
}
