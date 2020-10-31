#include <xt-cpp.hpp>
#include <iostream>
#include <cstdlib>
#include <cstring>

static void Callback(
  const Xt::Stream& stream, const void* input, void* output, int32_t frames,
  double time, uint64_t position, bool timeValid, uint64_t error, void* user) {

  memcpy(output, input, frames * 2 * 4);
}

int FullDuplexMain(int argc, char** argv) {

  Xt::Format format;
  Xt::Format int44100(Xt::Mix(44100, Xt::Sample::Int32), Xt::Channels(2, 0, 2, 0));
  Xt::Format int48000(Xt::Mix(48000, Xt::Sample::Int32), Xt::Channels(2, 0, 2, 0));
  Xt::Format float44100(Xt::Mix(44100, Xt::Sample::Float32), Xt::Channels(2, 0, 2, 0));
  Xt::Format float48000(Xt::Mix(48000, Xt::Sample::Float32), Xt::Channels(2, 0, 2, 0));

  Xt::Audio audio("", nullptr, nullptr, nullptr);
  auto system = Xt::Audio::SetupToSystem(Xt::Setup::ProAudio);
  std::unique_ptr<Xt::Service> service = Xt::Audio::GetService(system);
  if(!service)
    return 0;

  std::unique_ptr<Xt::Device> device = service->OpenDefaultDevice(true);
  if(!device)
    return 0;

  if(device->SupportsFormat(int44100))
    format = int44100;
  else if(device->SupportsFormat(int48000))
    format = int48000;
  else if(device->SupportsFormat(float44100))
    format = float44100;
  else if(device->SupportsFormat(float48000))
    format = float48000;
  else
    return 0;

  Xt::Buffer buffer = device->GetBuffer(format);
  std::unique_ptr<Xt::Stream> stream = device->OpenStream(format, true, buffer.current, Callback, nullptr, nullptr);
  stream->Start();
  std::cout << "Streaming full-duplex, press any key to continue...\n";
  std::string s;
  std::getline(std::cin, s);
  stream->Stop();
  return 0;
}