#include <xt-cpp.hpp>
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif
#include <fstream>
#include <iostream>

static void ReadLine() {
  std::string s;
  std::cout << "Press any key to continue...\n";
  std::getline(std::cin, s);
}

static int GetBufferSize(const Xt::Stream& stream, int frames) {
  const Xt::Format& format = stream.GetFormat();
  int sampleSize = Xt::Audio::GetSampleAttributes(format.mix.sample).size;
  return frames * format.channels.inputs * sampleSize;
}

static void XRun(int index, void* user) {
  // Don't do this.
  std::cout << "XRun on stream " << index << ".\n";
}

static void CaptureInterleaved(
  const Xt::Stream& stream, const void* input, void* output, int32_t frames,
  double time, uint64_t position, bool timeValid, uint64_t error, void* user) {

  if(frames > 0)
    // Don't do this.
    static_cast<std::ofstream*>(user)->write(static_cast<const char*>(input), GetBufferSize(stream, frames));
}

static void CaptureNonInterleaved(
  const Xt::Stream& stream, const void* input, void* output, int32_t frames,
  double time, uint64_t position, bool timeValid, uint64_t error, void* user) {

  if(frames > 0) {
    const Xt::Format& format = stream.GetFormat();
    int channels = format.channels.inputs;
    int sampleSize = Xt::Audio::GetSampleAttributes(format.mix.sample).size;
    for(int f = 0; f < frames; f++)
      for(int c = 0; c < channels; c++)
        // Don't do this.
        static_cast<std::ofstream*>(user)->write(&static_cast<char* const*>(input)[c][f * sampleSize], sampleSize);
  }
}

int CaptureAdvancedMain(int argc, char** argv) {

  Xt::Audio audio("", nullptr, nullptr, nullptr);
  Xt::Format format(Xt::Mix(44100, Xt::Sample::Int24), Xt::Channels(2, 0, 0, 0));

  auto system = Xt::Audio::SetupToSystem(Xt::Setup::ConsumerAudio);
  std::unique_ptr<Xt::Service> service = Xt::Audio::GetService(system);
  if(!service)
    return 0;  

  std::unique_ptr<Xt::Device> device = service->OpenDefaultDevice(false);
  if(!device || !device->SupportsFormat(format)) 
    return 0;

  std::unique_ptr<Xt::Stream> stream;
  Xt::Buffer buffer = device->GetBuffer(format);
  std::ofstream interleaved("xt-audio-interleaved.raw", std::ios::out | std::ios::binary);
  stream = device->OpenStream(format, true, buffer.current, CaptureInterleaved, XRun, &interleaved);
  stream->Start();
  std::cout << "Capturing interleaved...\n";
  ReadLine();
  stream->Stop();

  std::ofstream nonInterleaved("xt-audio-non-interleaved.raw", std::ios::out | std::ios::binary);
  stream = device->OpenStream(format, false, buffer.current, CaptureNonInterleaved, XRun, &nonInterleaved);
  stream->Start();
  std::cout << "Capturing non-interleaved...\n";
  ReadLine();
  stream->Stop();

  return 0;
}
