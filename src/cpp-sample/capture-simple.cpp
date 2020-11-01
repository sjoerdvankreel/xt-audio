#include <xt-cpp.hpp>
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif
#include <fstream>
#include <iostream>
#include <cmath>

static const int SampleSize = 3;
static const Xt::Format Format(Xt::Mix(44100, Xt::Sample::Int24), Xt::Channels(1, 0, 0, 0));

static void Capture(
  const Xt::Stream& stream, const void* input, void* output, int32_t frames,
  double time, uint64_t position, bool timeValid, uint64_t error, void* user) {

  if(frames > 0)
    // Don't do this.
    static_cast<std::ofstream*>(user)->write(static_cast<const char*>(input), frames * SampleSize);
}

int CaptureSimpleMain(int argc, char** argv) {

  Xt::Audio audio("", nullptr, nullptr);  
  auto system = Xt::Audio::SetupToSystem(Xt::Setup::ConsumerAudio);
  std::unique_ptr<Xt::Service> service = Xt::Audio::GetService(system);
  if(!service)
    return 0;  

  std::unique_ptr<Xt::Device> device = service->OpenDefaultDevice(false);
  if(!device || !device->SupportsFormat(Format))
    return 0;

  Xt::Buffer buffer = device->GetBuffer(Format);
  std::ofstream recording("xt-audio.raw", std::ios::out | std::ios::binary);
  std::unique_ptr<Xt::Stream> stream = device->OpenStream(Format, true, buffer.current, Capture, nullptr, &recording);
  stream->Start();
#if _WIN32
  Sleep(1000);
#else
  usleep(1000 * 1000);
#endif
  stream->Stop();
  return 0;
}