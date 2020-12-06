#include <xt/cpp/XtCpp.hpp>

#include <thread>
#include <chrono>
#include <fstream>

static Xt::Channels const Channels(1, 0, 0, 0);
static Xt::Mix const Mix(44100, Xt::Sample::Int24);
static Xt::Format const Format(Mix, Channels);

static void 
OnBuffer(Xt::Stream const& stream, Xt::Buffer const& buffer, void* user) 
{
  auto os = static_cast<std::ofstream*>(user);
  char const* input = static_cast<char const*>(buffer.input);
  int32_t bytes = Xt::Audio::GetSampleAttributes(Mix.sample).size * buffer.frames;
  os->write(input, bytes);
}

int 
CaptureSimpleMain() 
{
  std::unique_ptr<Xt::Platform> platform = Xt::Audio::Init("", nullptr, nullptr);
  Xt::System system = Xt::Audio::SetupToSystem(Xt::Setup::ConsumerAudio);
  std::unique_ptr<Xt::Service> service = platform->GetService(system);
  if(!service) return 0;  

  std::unique_ptr<Xt::DeviceList> list = service->OpenDeviceList();
  int32_t defaultInput = list->GetDefault(false);
  if(defaultInput == -1) return 0;
  std::string id = list->GetId(defaultInput);
  std::unique_ptr<Xt::Device> device = service->OpenDevice(id);
  if (!device->SupportsFormat(Format)) return 0;

  double bufferSize = device->GetBufferSize(Format).current;
  Xt::StreamParams streamParams(true, OnBuffer, nullptr);
  Xt::DeviceStreamParams deviceParams(streamParams, Format, bufferSize);
  std::ofstream recording("xt-audio.raw", std::ios::out | std::ios::binary);
  std::unique_ptr<Xt::Stream> stream = device->OpenStream(deviceParams, &recording);
  stream->Start();
  std::this_thread::sleep_for(std::chrono::seconds(2));
  stream->Stop();

  return 0;
}