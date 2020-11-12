#include <XtCpp.hpp>
#include <thread>
#include <chrono>
#include <fstream>

static const Xt::Channels Channels(1, 0, 0, 0);
static const Xt::Mix Mix(44100, Xt::Sample::Int24);
static const Xt::Format Format(Mix, Channels);

static void Capture(const Xt::Stream& stream, const Xt::Buffer& buffer, void* user) 
{
  auto os = static_cast<std::ofstream*>(user);
  const char* input = static_cast<const char*>(buffer.input);
  int32_t size = Xt::Audio::GetSampleAttributes(Mix.sample).size;
  os->write(input, buffer.frames * size);
}

int CaptureSimpleMain() 
{
  Xt::Audio audio("", nullptr, nullptr);  
  Xt::System system = Xt::Audio::SetupToSystem(Xt::Setup::ConsumerAudio);
  std::unique_ptr<Xt::Service> service = Xt::Audio::GetService(system);
  if(!service) return 0;  
  std::unique_ptr<Xt::Device> device = service->OpenDefaultDevice(false);
  if(!device || !device->SupportsFormat(Format)) return 0;
  double bufferSize = device->GetBufferSize(Format).current;
  Xt::StreamParams params(Format, true, bufferSize, Capture, nullptr);
  std::ofstream recording("xt-audio.raw", std::ios::out | std::ios::binary);
  std::unique_ptr<Xt::Stream> stream = device->OpenStream(params, &recording);
  stream->Start();
  std::this_thread::sleep_for(std::chrono::seconds(2));
  stream->Stop();
  return 0;
}