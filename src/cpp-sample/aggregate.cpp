#include <xt-cpp.hpp>
#include <iostream>
#include <cstdlib>
#include <cstring>

static void ReadLine() {
  std::string s;
  std::cout << "Press any key to continue...\n";
  std::getline(std::cin, s);
}

static void XRun(int index, void* user) {
  // Don't do this.
  std::cout << "XRun on stream " << index << ", user = " << static_cast<char*>(user) << ".\n";
}

static void Aggregate(
  const Xt::Stream& stream, const void* input, void* output, int32_t frames,
  double time, uint64_t position, bool timeValid, uint64_t error, void* user) {

  const Xt::Format& format = stream.GetFormat();
  Xt::Attributes attrs = Xt::Audio::GetSampleAttributes(format.mix.sample);
  memcpy(output, input, frames * format.inputs * attrs.size);
}

int AggregateMain(int argc, char** argv) {

  double bufferSizes[2];
  Xt::Device* devices[2];
  Xt::Channels channels[2];
  Xt::Mix mix(48000, Xt::Sample::Int16);
  Xt::Format inputFormat(mix, 2, 0, 0, 0);
  Xt::Channels inputChannels(2, 0, 0, 0);
  Xt::Format outputFormat(mix, 0, 0, 2, 0);
  Xt::Channels outputChannels(0, 0, 2, 0);

  Xt::Audio audio("", nullptr, nullptr, nullptr);
  std::unique_ptr<Xt::Service> service = Xt::Audio::GetServiceBySetup(Xt::Setup::SystemAudio);
  std::unique_ptr<Xt::Device> input = service->OpenDefaultDevice(false);
  std::unique_ptr<Xt::Device> output = service->OpenDefaultDevice(true);
  if(input && input->SupportsFormat(inputFormat)
    && output && output->SupportsFormat(outputFormat)) {

    Xt::Buffer buffer = input->GetBuffer(inputFormat);
    devices[0] = input.get();
    devices[1] = output.get();
    channels[0] = inputChannels;
    channels[1] = outputChannels;
    bufferSizes[0] = buffer.min;
    bufferSizes[1] = buffer.min;
    std::unique_ptr<Xt::Stream> stream = service->AggregateStream(devices, channels,
      bufferSizes, 2, mix, true, *output, Aggregate, XRun, const_cast<char*>("user-data"));
    stream->Start();
    std::cout << "Streaming aggregate, press any key to continue...\n";
    ReadLine();
    stream->Stop();
  }
  return 0;
}
