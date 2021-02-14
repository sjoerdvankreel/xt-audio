#include <xt/XtAudio.hpp>

#include <thread>
#include <chrono>
#include <cstring>
#include <iostream>

// Normally don't do I/O in the callback.
static void 
OnXRun(Xt::Stream const& stream, int32_t index, void* user) 
{ std::cout << "XRun on device " << index << ".\n"; }

static uint32_t 
OnBuffer(Xt::Stream const& stream, Xt::Buffer const& buffer, void* user) 
{
  int32_t bytes = buffer.frames * 2 * 4;
  std::memcpy(buffer.output, buffer.input, bytes);
  return 0;
}

static void
OnRunning(Xt::Stream const& stream, bool running, uint64_t error, void* user)
{ 
  char const* evt = running? "Started": "Stopped";
  std::cout << "Stream event: " << evt << ", new state: " << stream.IsRunning() << ".\n"; 
  if(error != 0) std::cout << Xt::Audio::GetErrorInfo(error) << ".\n";
}

int 
FullDuplexMain() 
{
  Xt::Format format;
  Xt::Format int44100(Xt::Mix(44100, Xt::Sample::Int32), Xt::Channels(2, 0, 2, 0));
  Xt::Format int48000(Xt::Mix(48000, Xt::Sample::Int32), Xt::Channels(2, 0, 2, 0));
  Xt::Format float44100(Xt::Mix(44100, Xt::Sample::Float32), Xt::Channels(2, 0, 2, 0));
  Xt::Format float48000(Xt::Mix(48000, Xt::Sample::Float32), Xt::Channels(2, 0, 2, 0));

  std::unique_ptr<Xt::Platform> platform = Xt::Audio::Init("", nullptr);
  Xt::System system = platform->SetupToSystem(Xt::Setup::ProAudio);
  std::unique_ptr<Xt::Service> service = platform->GetService(system);
  if(!service || (service->GetCapabilities() & Xt::ServiceCapsFullDuplex) == 0) return 0;

  std::optional<std::string> id = service->GetDefaultDeviceId(true);
  if(!id.has_value()) return 0;
  std::unique_ptr<Xt::Device> device = service->OpenDevice(id.value());

  if(device->SupportsFormat(int44100)) format = int44100;
  else if(device->SupportsFormat(int48000)) format = int48000;
  else if(device->SupportsFormat(float44100)) format = float44100;
  else if(device->SupportsFormat(float48000)) format = float48000;
  else return 0;

  double bufferSize = device->GetBufferSize(format).current;
  Xt::StreamParams streamParams(true, OnBuffer, OnXRun, OnRunning);
  Xt::DeviceStreamParams deviceParams(streamParams, format, bufferSize);
  std::unique_ptr<Xt::Stream> stream = device->OpenStream(deviceParams, nullptr);
  stream->Start();
  std::this_thread::sleep_for(std::chrono::seconds(2));
  stream->Stop();

  return 0;
}