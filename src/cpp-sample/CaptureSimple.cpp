#include <XtCpp.hpp>
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
  const Xt::Stream& stream, const Xt::Buffer& buffer, void* user) {

  if(buffer.frames > 0)
    // Don't do this.
    static_cast<std::ofstream*>(user)->write(static_cast<const char*>(buffer.input), buffer.frames * SampleSize);
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

  Xt::BufferSize size = device->GetBufferSize(Format);
  std::ofstream recording("xt-audio.raw", std::ios::out | std::ios::binary);
  std::unique_ptr<Xt::Stream> stream = device->OpenStream(Format, true, size.current, Capture, nullptr, &recording);
  stream->Start();
#if _WIN32
  Sleep(1000);
#else
  usleep(1000 * 1000);
#endif
  stream->Stop();
  return 0;
}