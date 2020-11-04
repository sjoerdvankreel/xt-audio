#ifdef __linux__
#include "XtLinux.hpp"

#ifdef XT_DISABLE_JACK
void XtlInitJack() { }
void XtlTerminateJack() { }
const XtService* XtiServiceJack = nullptr;
#else // XT_DISABLE_JACK

#include <jack/jack.h>
#include <vector>
#include <memory>
#include <sstream>

// ---- local ----

template <typename T>
struct JackPtr {
  T* const p;
  ~JackPtr() { jack_free(p); }
  JackPtr(T* p): p(p) { XT_ASSERT(p != nullptr); }
};

struct XtJackClient {
  jack_client_t* client;
  XtJackClient(const XtJackClient&) = delete;
  XtJackClient& operator=(const XtJackClient&) = delete;
  
  XtJackClient(XtJackClient&& rhs): client(rhs.client)
  { rhs.client = nullptr; }

  XtJackClient(jack_client_t* client): client(client) {}

  XtJackClient& operator=(XtJackClient&& rhs)
  { client = rhs.client; rhs.client = nullptr; return *this; }

  ~XtJackClient() 
  { if(client != nullptr) jack_client_close(client); }
};

struct XtJackPort {
  jack_port_t* port;
  jack_client_t* client;  
  const char* connectTo;

  XtJackPort(const XtJackPort&) = delete;
  XtJackPort& operator=(const XtJackPort&) = delete;

  XtJackPort(XtJackPort&& rhs): 
  port(rhs.port), client(rhs.client), connectTo(rhs.connectTo)
  { rhs.port = nullptr; }

  XtJackPort(jack_client_t* client, jack_port_t* port):
  port(port), client(client), connectTo(nullptr)
  { XT_ASSERT(client != nullptr); XT_ASSERT(port != nullptr); }

  ~XtJackPort() 
  { if(port != nullptr) jack_port_unregister(client, port); }

  XtJackPort& operator=(XtJackPort&& rhs)  
  { client = rhs.client; port = rhs.port; connectTo = rhs.connectTo; rhs.port = nullptr; return *this; }
};

struct XtJackConnection {
  const char* dest;
  const char* source;
  jack_client_t* client;  

  XtJackConnection(const XtJackConnection&) = delete;
  XtJackConnection& operator=(const XtJackConnection&) = delete;

  XtJackConnection(XtJackConnection&& rhs):
  dest(rhs.dest), source(rhs.source), client(rhs.client)
  { rhs.source = nullptr; }

  ~XtJackConnection()
  { if(source != nullptr) jack_disconnect(client, source, dest); }

  XtJackConnection(jack_client_t* client, const char* source, const char* dest):
  dest(dest), source(source), client(client)
  { XT_ASSERT(client != nullptr); XT_ASSERT(source != nullptr); XT_ASSERT(dest != nullptr); }

  XtJackConnection& operator=(XtJackConnection&& rhs)
  { dest = rhs.dest; source = rhs.source; client = rhs.client; rhs.source = nullptr; return *this; }
};

// ---- forward ----

XT_DECLARE_SERVICE(JACK, Jack);

struct JackDevice: public XtDevice {
  const XtJackClient client;
  XT_IMPLEMENT_DEVICE(JACK);
  JackDevice(XtJackClient&& c):
  client(std::move(c)) { XT_ASSERT(client.client != nullptr); }
};

struct JackStream: public XtStream {
  const XtJackClient client;
  std::vector<XtJackPort> inputs;
  std::vector<XtJackPort> outputs;
  std::vector<void*> inputChannels;
  std::vector<void*> outputChannels;
  std::vector<XtJackConnection> connections;
  XT_IMPLEMENT_CALLBACK_STREAM(JACK);

  JackStream(XtJackClient&& client, 
    std::vector<XtJackPort>&& inputs, std::vector<XtJackPort>&& outputs,
    size_t inputCount, size_t outputCount, size_t sampleSize, size_t bufferFrames):
  client(std::move(client)), inputs(std::move(inputs)), outputs(std::move(outputs)), 
  inputChannels(inputCount, nullptr), outputChannels(outputCount, nullptr),
  connections() { XT_ASSERT(this->client.client != nullptr); }
};

// ---- local ----

void JackSilentCallback(const char*) {
}

void JackErrorCallback(const char* msg) {
  XT_TRACE("JACK: %s", msg);
}

static int32_t CountPorts(jack_client_t* client, XtBool output) {
  int32_t count = 0;
  unsigned long flag = output? JackPortIsInput: JackPortIsOutput;
  JackPtr<const char*> ports(jack_get_ports(client, nullptr, JACK_DEFAULT_AUDIO_TYPE, flag));
  while(ports.p[count] != nullptr)
    count++;
  return count;
}

static XtFault CreatePorts(jack_client_t* client, uint32_t channels, uint64_t mask, 
  unsigned long xtFlag, unsigned long jackFlag, const char* name, std::vector<XtJackPort>& result) {

  const char* type = JACK_DEFAULT_AUDIO_TYPE;
  for(int32_t i = 0; i < channels; i++) {
    std::ostringstream oss;
    oss << name << (i + 1);
    unsigned long flags = xtFlag | JackPortIsTerminal;
    jack_port_t* port = jack_port_register(client, oss.str().c_str(), type, flags, 0);
    if(port == nullptr)
      return ENOENT;
    result.emplace_back(XtJackPort(client, port));
  }

  JackPtr<const char*> jackPorts(jack_get_ports(client, nullptr, type, jackFlag));
  if(mask == 0)
    for(int32_t i = 0; i < channels; i++)
      result[i].connectTo = jackPorts.p[i];
  else
    for(int32_t i = 0, j = 0; i < 64; i++)
      if(mask & (1ULL << i))
        result[j++].connectTo = jackPorts.p[i];
  return 0;
}

static int XRunCallback(void* arg) {
  static_cast<JackStream*>(arg)->ProcessXRun();
  return 0;
}

static int ProcessCallback(jack_nframes_t frames, void* arg) {
  
  void* input;
  void* output;
  float period;
  double time = 0.0;
  uint64_t position = 0;
  XtBool timeValid = XtFalse;
  jack_nframes_t jackPosition;
  jack_time_t jackTime, nextTime;
  JackStream* s = static_cast<JackStream*>(arg);
  int32_t sampleSize = XtiGetSampleSize(s->format.mix.sample);

  input = s->inputs.size() == 0? nullptr: &s->inputChannels[0];
  output = s->outputs.size() == 0? nullptr: &s->outputChannels[0];  
  if(jack_get_cycle_times(s->client.client, &jackPosition, &jackTime, &nextTime, &period) == 0) {
    timeValid = XtTrue;
    position = jackPosition;
    time = jackTime / 1000.0;
  }

  for(int32_t i = 0; i < s->inputs.size(); i++)
    s->inputChannels[i] = jack_port_get_buffer(s->inputs[i].port, frames);
  for(int32_t i = 0; i < s->outputs.size(); i++)
    s->outputChannels[i] = jack_port_get_buffer(s->outputs[i].port, frames);
  s->ProcessCallback(input, output, frames, time, position, timeValid, 0);
  return 0;
}

// ---- linux ----

void XtlInitJack() {
  jack_set_error_function(&JackErrorCallback);
}

void XtlTerminateJack() {
  jack_set_error_function(&JackSilentCallback);
}

// ---- service ----

XtFault JackService::GetFormatFault() const {
  return EINVAL;
}

XtCapabilities JackService::GetCapabilities() const {
  return static_cast<XtCapabilities>(
    XtCapabilitiesTime | 
    XtCapabilitiesFullDuplex | 
    XtCapabilitiesChannelMask | 
    XtCapabilitiesXRunDetection);
}

XtCause JackService::GetFaultCause(XtFault fault) const {
  return XtlPosixErrorToCause(fault);
}

const char* JackService::GetFaultText(XtFault fault) const {
  return strerror(fault);
}

XtFault JackService::GetDeviceCount(int32_t* count) const {
  XtJackClient client(jack_client_open(XtiId, JackNoStartServer, nullptr));
  *count = client.client == nullptr? 0: 1;
  return 0;
}

XtFault JackService::OpenDevice(int32_t index, XtDevice** device) const {  
  XtJackClient client(jack_client_open(XtiId, JackNoStartServer, nullptr));
  if(client.client == nullptr)
    return ESRCH;
  *device = new JackDevice(std::move(client));
  return 0;
}

XtFault JackService::OpenDefaultDevice(XtBool output, XtDevice** device) const { 
  XtJackClient client(jack_client_open(XtiId, JackNoStartServer, nullptr));
  if(client.client != nullptr)
    *device = new JackDevice(std::move(client));
  return 0;
}

// ---- device ----

XtFault JackDevice::ShowControlPanel() {
  return 0;
}

XtFault JackDevice::GetName(char** name) const {
  *name = strdup("JACK");
  return 0;
}

XtFault JackDevice::SupportsAccess(XtBool interleaved, XtBool* supports) const {
  *supports = !interleaved;
  return 0;
}

XtFault JackDevice::GetMix(XtMix** mix) const {
  *mix = static_cast<XtMix*>(malloc(sizeof(XtMix)));
  (*mix)->sample = XtSampleFloat32;
  (*mix)->rate = jack_get_sample_rate(client.client);
  return 0;
}

XtFault JackDevice::GetChannelCount(XtBool output, int32_t* count) const {
  *count = CountPorts(client.client, output);
  return 0;
}

XtFault JackDevice::GetBufferSize(const XtFormat* format, XtBufferSize* size) const {  
  jack_nframes_t rate = jack_get_sample_rate(client.client);
  size->current = jack_get_buffer_size(client.client) * 1000.0 / rate;
  size->min = size->current;
  size->max = size->current;
  return 0;
}

XtFault JackDevice::GetChannelName(XtBool output, int32_t index, char** name) const {
  unsigned long flag = output? JackPortIsInput: JackPortIsOutput;
  JackPtr<const char*> ports(jack_get_ports(client.client, nullptr, JACK_DEFAULT_AUDIO_TYPE, flag));
  if(index >= CountPorts(client.client, output))
    return ENODEV;
  *name = strdup(ports.p[index]);
  return 0;
}

XtFault JackDevice::SupportsFormat(const XtFormat* format, XtBool* supports) const {
  if(format->mix.sample != XtSampleFloat32)
    return 0;
  if(format->channels.inputs > CountPorts(client.client, XtFalse))
    return 0;
  if(format->channels.outputs > CountPorts(client.client, XtTrue))
    return 0;
  if(format->mix.rate != jack_get_sample_rate(client.client))
    return 0;
  for(int32_t i = CountPorts(client.client, XtFalse); i < 64; i++)
    if((format->channels.inMask & (1ULL << i)) != 0)
      return 0;
  for(int32_t i = CountPorts(client.client, XtTrue); i < 64; i++)
    if((format->channels.outMask & (1ULL << i)) != 0)
      return 0;
  *supports = XtTrue;
  return 0;
}

XtFault JackDevice::OpenStream(const XtFormat* format, XtBool interleaved, double bufferSize, 
                               bool secondary, XtStreamCallback callback, void* user, XtStream** stream) {
  
  XtFault fault;
  jack_client_t* c;
  size_t bufferFrames, sampleSize;
  std::unique_ptr<JackStream> result;

  c = jack_client_open(XtiId, JackNoStartServer, nullptr);
  if(c == nullptr)
    return ESRCH;
  XtJackClient streamClient(c);
  
  std::vector<XtJackPort> inputs, outputs;
  if((fault = CreatePorts(c, format->channels.inputs, format->channels.inMask, 
    JackPortIsInput, JackPortIsOutput, "inputs", inputs)) != 0)
    return fault;
  if((fault = CreatePorts(c, format->channels.outputs, format->channels.outMask, 
    JackPortIsOutput, JackPortIsInput, "outputs", outputs)) != 0)
    return fault;

  sampleSize = XtiGetSampleSize(format->mix.sample);
  bufferFrames = jack_get_buffer_size(streamClient.client);
  result.reset(new JackStream(std::move(streamClient), std::move(inputs), std::move(outputs), 
    format->channels.inputs, format->channels.outputs, bufferFrames, sampleSize));
  if((fault = jack_set_xrun_callback(c, &XRunCallback, result.get())) != 0)
    return fault;
  if((fault = jack_set_process_callback(c, &ProcessCallback, result.get())) != 0)
    return fault;
  *stream = result.release();
  return 0;
}

// ---- stream ----

XtFault JackStream::Stop() {
  connections.clear();
  return jack_deactivate(client.client);
}

XtFault JackStream::GetFrames(int32_t* frames) const {
  *frames = jack_get_buffer_size(client.client);
  return 0;
}

XtFault JackStream::GetLatency(XtLatency* latency) const {
  return 0;
}

XtFault JackStream::Start() {
  
  XtFault fault;
  std::vector<XtJackConnection> connections;
  if((fault = jack_activate(client.client)) != 0)
    return fault;

  for(int32_t i = 0; i < format.channels.inputs; i++) {
    if((fault = jack_connect(client.client, 
      inputs[i].connectTo, jack_port_name(inputs[i].port))) != 0)
      return fault;
    connections.emplace_back(XtJackConnection(client.client, 
      inputs[i].connectTo, jack_port_name(inputs[i].port)));
  }

  for(int32_t i = 0; i < format.channels.outputs; i++) {
    if((fault = jack_connect(client.client, 
      jack_port_name(outputs[i].port), outputs[i].connectTo)) != 0)
      return fault;
    connections.emplace_back(XtJackConnection(client.client, 
      jack_port_name(outputs[i].port), outputs[i].connectTo));
  }
  
  this->connections = std::move(connections);
  return 0;
}

#endif // XT_DISABLE_JACK
#endif // __linux__