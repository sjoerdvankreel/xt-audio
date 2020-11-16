#include <xt/cpp/XtCpp.hpp>
#include <thread>
#include <chrono>
#include <cstring>

static void OnBuffer(const Xt::Stream& stream, const Xt::Buffer& buffer, void* user) 
{
  int32_t bytes = buffer.frames * 2 * 4;
  std::memcpy(buffer.output, buffer.input, bytes);
}

int FullDuplexMain() 
{
  Xt::Format format;
  Xt::Format int44100(Xt::Mix(44100, Xt::Sample::Int32), Xt::Channels(2, 0, 2, 0));
  Xt::Format int48000(Xt::Mix(48000, Xt::Sample::Int32), Xt::Channels(2, 0, 2, 0));
  Xt::Format float44100(Xt::Mix(44100, Xt::Sample::Float32), Xt::Channels(2, 0, 2, 0));
  Xt::Format float48000(Xt::Mix(48000, Xt::Sample::Float32), Xt::Channels(2, 0, 2, 0));

  auto audio = Xt::Audio::Init("", nullptr, nullptr);
  Xt::System system = Xt::Audio::SetupToSystem(Xt::Setup::ProAudio);
  std::unique_ptr<Xt::Service> service = Xt::Audio::GetService(system);
  if(!service) return 0;

  std::unique_ptr<Xt::Device> device = service->OpenDefaultDevice(true);
  if(!device) return 0;
  if(device->SupportsFormat(int44100)) format = int44100;
  else if(device->SupportsFormat(int48000)) format = int48000;
  else if(device->SupportsFormat(float44100)) format = float44100;
  else if(device->SupportsFormat(float48000)) format = float48000;
  else return 0;

  double bufferSize = device->GetBufferSize(format).current;
  Xt::StreamParams streamParams(true, OnBuffer, nullptr);
  Xt::DeviceStreamParams deviceParams(streamParams, format, bufferSize);
  std::unique_ptr<Xt::Stream> stream = device->OpenStream(deviceParams, nullptr);
  stream->Start();
  std::this_thread::sleep_for(std::chrono::seconds(2));
  stream->Stop();

  return 0;
}