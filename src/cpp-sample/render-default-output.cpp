#include <xt-cpp.hpp>
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif
#include <climits>
#include <iostream>

static const int Channels = 2;
static const Xt::Sample RenderSample = Xt::Sample::Int16;

static void RenderCallback(
  const Xt::Stream& stream, const void* input, void* output, int32_t frames, 
  double time, uint64_t position, bool timeValid, uint64_t error, void* user) {

  if (frames == 0)
    return;
  short* buffer = static_cast<short*>(output);
  for(int32_t f = 0; f < frames; f++)
    for(int32_t c = 0; c < Channels; c++)
      buffer[f * Channels + c] = static_cast<short>((rand() / static_cast<double>(RAND_MAX) * 2.0 - 1.0) * SHRT_MAX);
}

int RenderMain(int argc, char** argv) {

  Xt::Audio init("", nullptr, nullptr, nullptr);
  std::unique_ptr<Xt::Service> service = Xt::Audio::GetServiceBySetup(Xt::Setup::ConsumerAudio);
  std::unique_ptr<Xt::Device> device = service->OpenDefaultDevice(true);

  if(!device) {
    std::cout << "No default device found.\n";
    return 0;
  }

  Xt::Format format(Xt::Mix(44100, RenderSample), 0, 0, Channels, 0);
  if (!device->SupportsFormat(format)) {
    std::cout << "Format not supported.\n";
    return 0;
  }
  
  Xt::Buffer buffer = device->GetBuffer(format);
  std::unique_ptr<Xt::Stream> stream = device->OpenStream(format, buffer.current, &RenderCallback, nullptr);
  stream->Start();
#if _WIN32
    Sleep(1000);
#else
    usleep(1000 * 1000);
#endif
  stream->Stop();
  return 0;
}
