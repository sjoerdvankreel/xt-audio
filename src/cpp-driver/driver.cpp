#include <xt-cpp.hpp>
#include <cmath>
#include <ctime>
#include <climits>
#include <cstring>
#include <cassert>
#include <fstream>
#include <sstream>
#include <iostream>
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif // _WIN32

#define M_PI 3.14159265358979323846

static bool Auto = false;
static const bool EnableList = true;
static const int32_t AutoMillis = 333;
static const bool EnableStream = true;
static const bool EnableListFull = true;
static const double ToneFrequency = 660.0;
static const int32_t StreamIterations = 2;
static const int32_t AutoStreamMillis = -1;
static const char RecordFileName[] = "xt-recording";
static const Xt::Level Tracelevel = Xt::Level::Info;

struct StreamContext {
  double phase;
  double start;
  int32_t xRunCount; 
  uint64_t processed;
  std::ofstream recording;
};

static Xt::Sample Samples[] = { 
  Xt::Sample::UInt8,
  Xt::Sample::Int16,
  Xt::Sample::Int24,
  Xt::Sample::Int32,
  Xt::Sample::Float32
};
static int32_t Channels[] = { 1, 2, 6, 8 };
static int32_t Rates[] = { 22050, 44100, 48000, 96000 };

static void OnFatalCallback() {
  throw std::logic_error("Driver fatal error.");
}

static bool ReadLine(const char* message) {
  std::string s;
  std::cout << message;
  std::cout << "Enter q to quit, any key to continue...\n";
  std::getline(std::cin, s);
  return s != "q";
}

static void OnTraceCallback(Xt::Level level, const std::string& message) {
  if(level >= Tracelevel)
    std::cout << "-- " << level << ": " << message << "\n";
}

static void ListChannels(const Xt::Device& device, bool output) {
  if(!EnableListFull)
    return;
  int32_t count = device.GetChannelCount(output);
  std::cout << "    " << (output? "Output": "Input") << " channels: " << count << ": ";
  for(int32_t c = 0; c < count; c++)
    std::cout << " " << device.GetChannelName(output, c) << ",";
  std::cout << "\n";
}

static void ListFormats(const Xt::Device& device) {
  if(!EnableListFull)
    return;
  for(int32_t r = 0; r < sizeof(Rates) / sizeof(Rates[0]); r++)
    for(int32_t s = 0; s < sizeof(Samples) / sizeof(Samples[0]); s++)
      for(int32_t c = 0; c < sizeof(Channels) / sizeof(Channels[0]); c++) {
        Xt::Format format(Xt::Mix(Rates[r], Samples[s]), Channels[c], 0, 0, 0);
        if(device.SupportsFormat(format))
          std::cout << "    Supports: " << format << " (" << device.GetBuffer(format) << ")\n";
        format.inputs = 0;
        format.outputs = Channels[c];
        if(device.SupportsFormat(format))
          std::cout << "    Supports: " << format << " (" << device.GetBuffer(format) << ")\n";
        format.inputs = Channels[c];
        if(device.SupportsFormat(format))
          std::cout << "    Supports: " << format << " (" << device.GetBuffer(format) << ")\n";
      }
}

static bool ListAsioControlPanel(Xt::Device& device) {
  if(Auto || !EnableListFull || device.GetSystem() != Xt::System::Asio)
    return true;
  device.ShowControlPanel();
  return ReadLine("Please close ASIO control panel.\n");
}

static bool ListDevice(Xt::Device& device) {
  std::cout << "    " << device.GetName() << " (" << device.GetSystem() << ")\n";
  if(EnableListFull && !ListAsioControlPanel(device))
    return false;
  std::unique_ptr<Xt::Mix> mix = device.GetMix();
  if(mix)
    std::cout << "    Current: " << *mix.get() << "\n";
  std::cout << "    Interleaved access: " << device.SupportsAccess(true) << "\n";
  std::cout << "    Non-interleaved access: " << device.SupportsAccess(false) << "\n";
  ListChannels(device, false);
  ListChannels(device, true);
  ListFormats(device);
  return true;
}

static bool ListService(Xt::Service& service) {
  std::unique_ptr<Xt::Device> defaultIn;
  std::unique_ptr<Xt::Device> defaultOut;
  std::cout << "  " << service.GetName() << " (" << service.GetSystem() << ")\n";
  std::cout << "  Capabilities: " << service.GetCapabilities() << "\n";
  std::cout << "  Default in:\n";
  defaultIn = service.OpenDefaultDevice(false);
  if(defaultIn && !ListDevice(*defaultIn))
    return false;
  std::cout << "  Default out:\n";
  defaultOut = service.OpenDefaultDevice(true);
  if(defaultOut && !ListDevice(*defaultOut))
    return false;
  int32_t count = service.GetDeviceCount();
  for(int32_t d = 0; d < count; d++) {
    std::cout << "  Device " << d << ":\n";
    std::unique_ptr<Xt::Device> device = service.OpenDevice(d);
    if(!ListDevice(*device.get()))
      return false;
  }
  return true;
}

static bool List() {
  if(!EnableList)
    return true;
  std::cout << "Win32: " << Xt::Audio::IsWin32() << "\n";
  
  auto pro = Xt::Audio::GetServiceBySetup(Xt::Setup::ProAudio);
  std::cout << Xt::Setup::ProAudio << ": " << (pro? pro->GetName(): "None") << "\n"; 
  auto system = Xt::Audio::GetServiceBySetup(Xt::Setup::SystemAudio);
  std::cout << Xt::Setup::SystemAudio << ": " << (system? system->GetName(): "None") << "\n"; 
  auto consumer = Xt::Audio::GetServiceBySetup(Xt::Setup::ConsumerAudio);
  std::cout << Xt::Setup::ConsumerAudio << ": " << (consumer? consumer->GetName(): "None") << "\n"; 
  int32_t count = Xt::Audio::GetServiceCount();
  for(int32_t s = 0; s < count; s++) {
    std::cout << "Service " << s << ":\n";
    if(!ListService(*Xt::Audio::GetServiceByIndex(s)))
      return false;
  }
  return true;
}

static void OutputSine(void* dest, Xt::Sample sample, double value) {
  int ivalue;
  switch(sample) {
  case Xt::Sample::Float32:
    *static_cast<float*>(dest) = static_cast<float>(value);
    break;
  case Xt::Sample::Int32:
    *static_cast<int*>(dest) = static_cast<int>(value * INT_MAX);
    break;
  case Xt::Sample::UInt8: 
    *static_cast<unsigned char*>(dest) 
      = static_cast<unsigned char>((value + 1.0) / 2.0 * UCHAR_MAX);
    break;
  case Xt::Sample::Int16:
    *static_cast<short*>(dest) = static_cast<short>(value * SHRT_MAX);
    break;
  case Xt::Sample::Int24:
    ivalue = static_cast<int>(value * INT_MAX);
    static_cast<unsigned char*>(dest)[0] = static_cast<unsigned char>((ivalue & 0x0000FF00) >> 8);
    static_cast<unsigned char*>(dest)[1] = static_cast<unsigned char>((ivalue & 0x00FF0000) >> 16);
    static_cast<unsigned char*>(dest)[2] = static_cast<unsigned char>((ivalue & 0xFF000000) >> 24);
    break;
  default:
    assert(false);
  }
}

static void OnXRunCallback(int32_t index, void* user) {
    StreamContext* ctx = static_cast<StreamContext*>(user);
    ctx->xRunCount++;
    std::cout << "XRun on index " << index 
              << ", count = " << ctx->xRunCount << "\n";
}

static void OnStreamCallback(
  const Xt::Stream& stream, const void* input, void* output, int32_t frames,
  double time, uint64_t position, bool timeValid, uint64_t error, void* user) {

  void* dest;
  double value;
  bool interleaved = stream.IsInterleaved();
  const Xt::Format& format = stream.GetFormat();
  StreamContext& ctx = *static_cast<StreamContext*>(user);
  int32_t outputs = format.outputs;
  int32_t sampleSize = Xt::Audio::GetSampleAttributes(format.mix.sample).size;
  int32_t bufferSizeBytes = frames * format.inputs * sampleSize;
  assert(format.inputs == 0 || format.outputs == 0 || format.inputs == format.outputs);
  
  if(error != 0) {
    std::cout << "Stream error: " << Xt::Print::ErrorToString(error) << "\n";
    return;
  }

  if(frames == 0)
    return;
  if(format.outputs == 0) {
    if(interleaved)
      ctx.recording.write(static_cast<const char*>(input), bufferSizeBytes);
    else
      for(int32_t f = 0; f < frames; f++)
        for(int32_t c = 0; c < format.inputs; c++)
          ctx.recording.write(&((reinterpret_cast<char* const*>(input))[c][f * sampleSize]), sampleSize);
  } else if(format.inputs != 0) {
    if(interleaved)
      memcpy(output, input, bufferSizeBytes);
    else
      for(int32_t c = 0; c < format.inputs; c++)
        memcpy(reinterpret_cast<char**>(output)[c], 
               reinterpret_cast<char* const*>(input)[c], 
               frames * sampleSize);
  } else {
    for(int32_t f = 0; f < frames; f++) {
      ctx.phase += ToneFrequency / format.mix.rate;
      if(ctx.phase > 1.0)
        ctx.phase = -1.0;
      value = sin(ctx.phase * M_PI) * 0.95;
      for(int32_t c = 0; c < format.outputs; c++) {
        if(interleaved)
          dest = &static_cast<unsigned char*>(output)[(f * outputs + c) * sampleSize];
        else
          dest = &(reinterpret_cast<unsigned char* const*>(output)[c][f * sampleSize]);
        OutputSine(dest, format.mix.sample, value);
      }
    }
  }

  if(ctx.start < 0.0)
    ctx.start = time;
  ctx.processed += frames;
  if(ctx.processed > format.mix.rate) {
    std::cout << "Time: " << (time - ctx.start) 
              << ", position: " << position
              << ", valid: " << timeValid
              << ", latency: " << stream.GetLatency() << "\n";
    ctx.processed = 0;
  }
}

static std::string GetRecordFileName(const Xt::Service& service, Xt::Device& device, 
                                     const Xt::Format& format, double bufferSize, bool interleaved) {

  time_t t = time(0);
  tm* now = localtime(&t);
  std::ostringstream result;
  result << RecordFileName;
  result << "-" << service.GetName();
  result << "-" << device.GetName();
  result << "-" << (now->tm_year + 1900) << "." << (now->tm_mon) << "." << (now->tm_mday);
  result << "." << (now->tm_hour) << "." << (now->tm_min) << "." << (now->tm_sec);
  result << "-" << format;
  result << "-" << bufferSize;
  result << "-" << interleaved;
  result << ".raw";
  return result.str();
}

static bool StreamIteration(Xt::Stream& stream, int32_t iter) {
  std::cout << "Streaming iteration " << (iter + 1) << "...\n";
  if(!Auto && AutoStreamMillis == -1 && !ReadLine("Waiting for stream to start...\n"))
      return false;
  stream.Start();
  if(!Auto && AutoStreamMillis == -1 && !ReadLine("Waiting for stream to stop...\n"))
      return false;
  if(Auto || AutoStreamMillis != -1)
#if _WIN32
    Sleep(Auto? AutoMillis: AutoStreamMillis);
#else
    usleep((Auto? AutoMillis: AutoStreamMillis) * 1000);
#endif // _WIN32
  stream.Stop();
  return true;
}

static bool StreamAccessMode(Xt::Service& service, Xt::Device& device, 
                             const Xt::Format& format, double bufferSize, bool interleaved) {

  int32_t frames;
  int32_t channelCount;
  bool thisDeviceIsInput;
  StreamContext context;
  context.phase = 0.0;
  context.start = -1.0;
  context.processed = 0;
  context.xRunCount = 0;
  double bufferSizes[2];
  Xt::Device* devices[2];
  Xt::Channels channels[2];
  Xt::Format otherFormat = Xt::Format();
  std::unique_ptr<Xt::Device> otherDevice;
  std::string fileName = GetRecordFileName(service, device, format, bufferSize, interleaved);

  memset(channels, 0, sizeof(channels));
  std::cout << "Streaming: " << service.GetName() << ": " << device.GetName();
  std::cout << ", format: " << format << ", buffer: " << bufferSize << ", interleaved: " << interleaved<< "\n";
  std::unique_ptr<Xt::Stream> stream = device.OpenStream(format, interleaved, bufferSize, &OnStreamCallback, &OnXRunCallback, &context);
  frames = stream->GetFrames();
  std::cout << "Latency: " << stream->GetLatency() << "\n";
  std::cout << "Buffer: " << frames << " (" << (frames * 1000.0 / format.mix.rate) << " ms) \n";

  if(format.outputs == 0)
    context.recording.open(fileName, std::ios::out | std::ios::binary);
  for(int32_t i = 0; i < StreamIterations; i++)
    if(!StreamIteration(*stream, i))
      return false;
  stream.reset(nullptr);

  if((service.GetCapabilities() & Xt::CapabilitiesFullDuplex) == 0) {
    thisDeviceIsInput = device.GetChannelCount(false) != 0;
    channelCount = thisDeviceIsInput? format.inputs: format.outputs;
    otherDevice = service.OpenDefaultDevice(thisDeviceIsInput);
    if(!otherDevice)
      return true;
    devices[0] = thisDeviceIsInput? &device: otherDevice.get();
    devices[1] = thisDeviceIsInput? otherDevice.get(): &device;
    channels[0].inputs = channelCount;
    channels[1].outputs = channelCount;
    bufferSizes[0] = bufferSize;
    bufferSizes[1] = bufferSize;
    otherFormat.mix = format.mix;
    otherFormat.inputs = thisDeviceIsInput? 0: channelCount;
    otherFormat.outputs = thisDeviceIsInput? channelCount: 0;
    if(otherDevice->SupportsFormat(otherFormat)) {
      std::cout << "Streaming aggregate, other device = " << otherDevice->GetName() << " ...\n";
      std::unique_ptr<Xt::Stream> aggregate = service.AggregateStream(
          devices, channels, bufferSizes, 2, format.mix, interleaved, 
          device, &OnStreamCallback, &OnXRunCallback, &context);
      for(int32_t i = 0; i < StreamIterations; i++)
        if(!StreamIteration(*aggregate, i))
          return false;
    }
  }
  
  return true;
}

static bool StreamBufferSize(Xt::Service& service, Xt::Device& device, 
                             const Xt::Format& format, double bufferSize) {

  if(!StreamAccessMode(service, device, format, bufferSize, true))
    return false;
  if(!StreamAccessMode(service, device, format, bufferSize, false))
    return false;
  return true;
}

static bool StreamFormat(Xt::Service& service, Xt::Device& device, const Xt::Format& format) {
  Xt::Buffer buffer = device.GetBuffer(format);
  if(!StreamBufferSize(service, device, format, buffer.min))
    return false;
  if(!StreamBufferSize(service, device, format, buffer.max))
    return false;
  if(!StreamBufferSize(service, device, format, buffer.current))
    return false;
  return true;
}

static bool StreamDevice(Xt::Service& service, Xt::Device& device) {
  for(int32_t r = 0; r < sizeof(Rates) / sizeof(Rates[0]); r++)
    for(int32_t s = 0; s < sizeof(Samples) / sizeof(Samples[0]); s++) {
      int32_t maxOutputs = 0;
      for(int32_t c = 0; c < sizeof(Channels) / sizeof(Channels[0]); c++) {
        Xt::Format format(Xt::Mix(Rates[r], Samples[s]), Channels[c], 0, 0, 0);
        if(device.SupportsFormat(format))
          if(!StreamFormat(service, device, format))
            return false;
        format.inputs = 0;
        format.outputs = Channels[c];
        if(device.SupportsFormat(format)) {
          maxOutputs = Channels[c] > maxOutputs? Channels[c]: maxOutputs;
          if(!StreamFormat(service, device, format))
            return false;
        }
        format.inputs = format.outputs;
        if(device.SupportsFormat(format))
          if(!StreamFormat(service, device, format))
            return false;
      }
      if(service.GetCapabilities() & Xt::CapabilitiesChannelMask)
        for(int32_t c = 0; c < maxOutputs; c++) {
          Xt::Format format(Xt::Mix(Rates[r], Samples[s]), 0, 0, 1, 1ULL << c);
          if(device.SupportsFormat(format))
            if(!StreamFormat(service, device, format))
              return false;
        }
    }
  return true;
}

static bool StreamService(Xt::Service& service) {
  std::unique_ptr<Xt::Device> defaultInput = service.OpenDefaultDevice(false);
  if(defaultInput)
    if(!StreamDevice(service, *defaultInput.get()))
      return false;
  std::unique_ptr<Xt::Device> defaultOutput = service.OpenDefaultDevice(true);
  if(defaultOutput)
    if(!StreamDevice(service, *defaultOutput.get()))
      return false;
  int32_t count = service.GetDeviceCount();
  for(int32_t d = 0; d < count; d++)
    if(!StreamDevice(service, *service.OpenDevice(d)))
      return false;
  return true;
}

static bool Stream() {
  if(!EnableStream)
    return true;
  int32_t count = Xt::Audio::GetServiceCount();
  for(int32_t s = 0; s < count; s++)
    if(!StreamService(*Xt::Audio::GetServiceByIndex(s)))
      return false;
  return true;
}

static int Driver() {

  try {
#ifdef _WIN32
    Xt::Audio audio("XT-Audio-Driver", ::GetConsoleWindow(), &OnTraceCallback, &OnFatalCallback);
#else
    Xt::Audio audio("XT-Audio-Driver", nullptr, &OnTraceCallback, &OnFatalCallback);
#endif // _WIN32
    std::cout << "Xt version: " << Xt::Audio::GetVersion() << "\n";
    if(!List())
      return 0;
    Stream();
    return 0;
  } catch(const Xt::Exception& e) {
    std::cout << "Error: " << e << "\n";
  } catch(const std::exception& e) {
    std::cout << e.what() << "\n";
  } catch(...) {
    std::cout << "Unknown error.\n";
  }
  return 1;
}

int main(int argc, char** argv) {
  if(argc > 1)
    Auto = true;
  int result = Driver();
  ReadLine("Waiting for exit...\n");
  return result;
}
