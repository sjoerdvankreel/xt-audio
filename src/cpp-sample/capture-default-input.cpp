#include <xt-cpp.hpp>
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif
#include <fstream>
#include <iostream>

static void CaptureCallback(
  const Xt::Stream& stream, const void* input, void* output, int32_t frames, 
  double time, uint64_t position, bool timeValid, uint64_t error, void* user) {

  const Xt::Format& format = stream.GetFormat();
  int32_t sampleSize = Xt::Audio::GetSampleAttributes(format.mix.sample).size;
  int32_t bufferSize = frames * format.inputs * sampleSize;

  if(frames > 0)
    // Don't do this.
    static_cast<std::ofstream*>(user)->write(static_cast<const char*>(input), bufferSize);
}

int CaptureMain(int argc, char** argv) {

  Xt::Audio init("", nullptr, nullptr, nullptr);
  std::unique_ptr<Xt::Service> service = Xt::Audio::GetServiceBySetup(Xt::Setup::ConsumerAudio);
  std::unique_ptr<Xt::Device> device = service->OpenDefaultDevice(false);

  if(!device) {
    std::cout << "No default device found.\n";
    return 0;
  }

  Xt::Format format(Xt::Mix(44100, Xt::Sample::Int24), 2, 0, 0, 0);
  if (!device->SupportsFormat(format)) {
    std::cout << "Format not supported.\n";
    return 0;
  }
  
  Xt::Buffer buffer = device->GetBuffer(format);
  std::ofstream recording("xt-audio.raw", std::ios::out | std::ios::binary);
  std::unique_ptr<Xt::Stream> stream = device->OpenStream(format, 
    true, buffer.current, &CaptureCallback, &recording);
  stream->Start();
#if _WIN32
    Sleep(1000);
#else
    usleep(1000 * 1000);
#endif
  stream->Stop();
  return 0;
}
