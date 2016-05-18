#include <xt-cpp.hpp>
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif
#include <climits>
#include <iostream>
#include <cmath>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

static double phase = 0.0;
static const double Frequency = 660.0;

static void ReadLine() {
  std::string s;
  std::cout << "Press any key to continue...\n";
  std::getline(std::cin, s);
}

static float NextSine(double sampleRate) {
  phase += Frequency / sampleRate;
  if(phase >= 1.0)
    phase = -1.0;
  return (float)sin(phase * M_PI);
}

static void RenderInterleaved(
  const Xt::Stream& stream, const void* input, void* output, int32_t frames,
  double time, uint64_t position, bool timeValid, uint64_t error, void* user) {

  Xt::Format format = stream.GetFormat();
  for(int f = 0; f < frames; f++) {
    float sine = NextSine(format.mix.rate);
    for(int c = 0; c < format.outputs; c++)
      ((float*)output)[f * format.outputs + c] = sine;
  }
}

static void RenderNonInterleaved(
  const Xt::Stream& stream, const void* input, void* output, int32_t frames,
  double time, uint64_t position, bool timeValid, uint64_t error, void* user) {

  Xt::Format format = stream.GetFormat();
  for(int f = 0; f < frames; f++) {
    float sine = NextSine(format.mix.rate);
    for(int c = 0; c < format.outputs; c++)
      ((float**)output)[c][f] = sine;
  }
}

int RenderAdvancedMain(int argc, char** argv) {

  Xt::Audio audio("", nullptr, nullptr, nullptr);
  std::unique_ptr< Xt::Service> service = Xt::Audio::GetServiceBySetup(Xt::Setup::ConsumerAudio);
  Xt::Format format(Xt::Mix(44100, Xt::Sample::Float32), 0, 0, 2, 0);
  std::unique_ptr<Xt::Device> device = service->OpenDefaultDevice(false);

  if(!device) {
    std::cout << "No default device found.\n";
    return 0;
  }

  if(!device->SupportsFormat(format)) {
    std::cout << "Format not supported.\n";
    return 0;
  }

  std::unique_ptr<Xt::Stream> stream;
  Xt::Buffer buffer = device->GetBuffer(format);

  stream = device->OpenStream(format, true, buffer.current, RenderInterleaved, nullptr);
  stream->Start();
  std::cout << "Rendering interleaved...\n";
  ReadLine();
  stream->Stop();

  device->OpenStream(format, false, buffer.current, RenderNonInterleaved, nullptr);
  stream->Start();
  std::cout << "Rendering non-interleaved...\n";
  ReadLine();
  stream->Stop();

  Xt::Format sendTo0(Xt::Mix(44100, Xt::Sample::Float32), 0, 0, 1, 1ULL << 0);
  stream = device->OpenStream(format, true, buffer.current, RenderInterleaved, nullptr);
  stream->Start();
  std::cout << "Rendering channel mask, channel 0...\n";
  ReadLine();
  stream->Stop();

  Xt::Format sendTo1(Xt::Mix(44100, Xt::Sample::Float32), 0, 0, 1, 1ULL << 1);
  stream = device->OpenStream(format, true, buffer.current, RenderInterleaved, nullptr);
  stream->Start();
  std::cout << "Rendering channel mask, channel 1...\n";
  ReadLine();
  stream->Stop();

  return 0;
}
