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
static const double Frequency = 440.0;

static void ReadLine() {
  std::string s;
  std::cout << "Press any key to continue...\n";
  std::getline(std::cin, s);
}

static float NextSine(double sampleRate) {
  phase += Frequency / sampleRate;
  if(phase >= 1.0)
    phase = -1.0;
  return (float)sin(2.0 * phase * M_PI);
}

static void XRun(int index, void* user) {
  // Don't do this.
  std::cout << "XRun on stream " << index << ", user = " << static_cast<char*>(user) << ".\n";
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
  Xt::Format format(Xt::Mix(44100, Xt::Sample::Float32), 0, 0, 2, 0);

  std::unique_ptr<Xt::Service> service = Xt::Audio::GetServiceBySetup(Xt::Setup::ConsumerAudio);
  if(!service)
    return 0;

  std::unique_ptr<Xt::Device> device = service->OpenDefaultDevice(true);
  if(!device || !device->SupportsFormat(format))
    return 0;

  std::unique_ptr<Xt::Stream> stream;
  Xt::Buffer buffer = device->GetBuffer(format);
  stream = device->OpenStream(format, true, buffer.current, RenderInterleaved, XRun, const_cast<char*>("user-data"));
  stream->Start();
  std::cout << "Rendering interleaved...\n";
  ReadLine();
  stream->Stop();

  stream = device->OpenStream(format, false, buffer.current, RenderNonInterleaved, XRun, const_cast<char*>("user-data"));
  stream->Start();
  std::cout << "Rendering non-interleaved...\n";
  ReadLine();
  stream->Stop();

  Xt::Format sendTo0(Xt::Mix(44100, Xt::Sample::Float32), 0, 0, 1, 1ULL << 0);
  stream = device->OpenStream(sendTo0, true, buffer.current, RenderInterleaved, XRun, const_cast<char*>("user-data"));
  stream->Start();
  std::cout << "Rendering channel mask, channel 0...\n";
  ReadLine();
  stream->Stop();

  Xt::Format sendTo1(Xt::Mix(44100, Xt::Sample::Float32), 0, 0, 1, 1ULL << 1);
  stream = device->OpenStream(sendTo1, true, buffer.current, RenderInterleaved, XRun, const_cast<char*>("user-data"));
  stream->Start();
  std::cout << "Rendering channel mask, channel 1...\n";
  ReadLine();
  stream->Stop();
  return 0;
}
