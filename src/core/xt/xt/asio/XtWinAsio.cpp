#if XT_ENABLE_ASIO
#include <xt/asio/Shared.hpp>
#include <memory>
#include <vector>
#include <atomic>

static const double XtAsioNsPerMs = 1000000.0;


#define XT_TO_UINT64(lo, hi) ((uint64_t)(lo) | ((uint64_t)(hi) << 32))



// ---- forward ----



struct AsioStream: public XtStream {
  bool issueOutputReady;
  const long bufferSize;
  ASIOCallbacks callbacks;
  AsioDevice* const device;
  std::atomic<int32_t> running;
  std::atomic<int32_t> insideCallback;
  std::vector<void*> inputChannels;
  std::vector<void*> outputChannels;
  std::vector<ASIOBufferInfo> buffers;
  const std::unique_ptr<asmjit::JitRuntime> runtime;
  XT_IMPLEMENT_STREAM();
  XT_IMLEMENT_STREAM_SYSTEM(ASIO);

  ~AsioStream();
  AsioStream(AsioDevice* d, const XtFormat& format, 
    size_t bufferSize, const std::vector<ASIOBufferInfo>& buffers):
  XtStream(), issueOutputReady(true), 
  bufferSize(static_cast<long>(bufferSize)), 
  callbacks(), device(d), running(0), insideCallback(0),
  inputChannels(static_cast<size_t>(format.channels.inputs), nullptr),
  outputChannels(static_cast<size_t>(format.channels.outputs), nullptr),
  buffers(buffers), runtime(std::make_unique<asmjit::JitRuntime>()) {}
};

// ---- local ----





static void CreateMaskBufferInfos(
  std::vector<ASIOBufferInfo>& infos, ASIOBool input, uint64_t mask) {

  for(int i = 0; i < 64; i++)
    if(mask & (1ULL << i)) {
      ASIOBufferInfo info = { 0 };
      info.isInput = input;
      info.channelNum = i;
      infos.push_back(info);
    }
}

static void CreateChannelBufferInfos(
  std::vector<ASIOBufferInfo>& infos, ASIOBool input, int32_t channels) {

  for(int i = 0; i < channels; i++) {
    ASIOBufferInfo info = { 0 };
    info.isInput = input;
    info.channelNum = i;
    infos.push_back(info);
  }
}

static void CreateBufferInfos(
  std::vector<ASIOBufferInfo>& infos, ASIOBool input, int32_t channels, uint64_t mask) {

  if(mask == 0)
    CreateChannelBufferInfos(infos, input, channels);
  else
    CreateMaskBufferInfos(infos, input, mask);
}

// ---- local ----

static void XT_ASIO_CALL SampleRateDidChange(ASIOSampleRate) { 
}

static long XT_ASIO_CALL AsioMessage(long selector, long, void*, double*) { 
  return selector == kAsioResetRequest; 
}

static ASIOTime* XT_ASIO_CALL BufferSwitchTimeInfo(
  void* ctx, ASIOTime* asioTime, long index, ASIOBool) {

  void* input;
  void* output;
  double time = 0.0;
  uint64_t position = 0;
  XtBool timeValid = XtFalse;
  AsioTimeInfo& info = asioTime->timeInfo;
  AsioStream* s = static_cast<AsioStream*>(ctx);

  if(s->running.load() != 1)
    return nullptr;
  if(!XtiCompareExchange(s->insideCallback, 0, 1))
    return nullptr;

  if(info.flags & kSamplePositionValid && info.flags & kSystemTimeValid) {
    timeValid = XtTrue;
    position = XT_TO_UINT64(info.samplePosition.lo, info.samplePosition.hi);
    time = XT_TO_UINT64(info.systemTime.lo, info.systemTime.hi) / XtAsioNsPerMs;
  }

  input = s->_params.format.channels.inputs > 0? &s->inputChannels[0]: nullptr;
  output = s->_params.format.channels.outputs > 0? &s->outputChannels[0]: nullptr;
  for(int32_t i = 0; i < s->_params.format.channels.inputs; i++)
    s->inputChannels[i] = s->buffers[i].buffers[index];
  for(int32_t i = 0; i < s->_params.format.channels.outputs; i++)
    s->outputChannels[i] = s->buffers[s->_params.format.channels.inputs + i].buffers[index];

  XtBuffer buffer = { 0 };
  buffer.input = input;
  buffer.output = output;
  buffer.frames = s->bufferSize;
  buffer.time = time;
  buffer.position = position;
  buffer.timeValid = timeValid;
  s->OnBuffer(&buffer);
  if(s->issueOutputReady)
    s->issueOutputReady = s->device->_asio->outputReady() == ASE_OK;

  XT_ASSERT(XtiCompareExchange(s->insideCallback, 1, 0));
  return nullptr; 
}

static void XT_ASIO_CALL BufferSwitch(void* ctx, long index, ASIOBool direct) {

  ASIOTime time = { 0 };
  AsioTimeInfo& info = time.timeInfo;
  AsioStream* stream = static_cast<AsioStream*>(ctx);
	if(stream->device->_asio->getSamplePosition(&info.samplePosition, &info.systemTime) == ASE_OK)
		info.flags = kSystemTimeValid | kSamplePositionValid;
	BufferSwitchTimeInfo(ctx, &time, index, direct);
}

// ---- local ----

// ---- service ----

// ---- device ----


XtFault AsioDevice::OpenStreamCore(const XtDeviceStreamParams* params, bool secondary, void* user, XtStream** stream) {
  
  double wantedSize;
  long asioBufferSize;
  ASIOSampleRate rate;
  std::vector<ASIOBufferInfo> buffers;
  long min, max, preferred, granularity;

  if(_streamOpen)
    return static_cast<XtFault>(DRVERR_DEVICE_ALREADY_OPEN);
  XT_VERIFY_ASIO(_asio->getSampleRate(&rate));
  XT_VERIFY_ASIO(_asio->getBufferSize(&min, &max, &preferred, &granularity));

  wantedSize = params->bufferSize / 1000.0 * rate;
  if(wantedSize < min)
    asioBufferSize = min;
  else if(wantedSize > max)
    asioBufferSize = max;
  else {
    asioBufferSize = min;
    while(asioBufferSize < wantedSize)
      asioBufferSize += granularity == -1? asioBufferSize: granularity;
    if(asioBufferSize > max)
      asioBufferSize = max;
  }

  CreateBufferInfos(buffers, ASIOTrue, params->format.channels.inputs, params->format.channels.inMask);
  CreateBufferInfos(buffers, ASIOFalse, params->format.channels.outputs, params->format.channels.outMask);
  auto result = std::make_unique<AsioStream>(this, params->format, asioBufferSize, buffers);
  result->callbacks.asioMessage = &AsioMessage;
  result->callbacks.sampleRateDidChange = &SampleRateDidChange;
  result->callbacks.bufferSwitch = JitBufferSwitch(result->runtime.get(), &BufferSwitch, result.get());
  result->callbacks.bufferSwitchTimeInfo = JitBufferSwitchTimeInfo(result->runtime.get(), &BufferSwitchTimeInfo, result.get());
  XT_VERIFY_ASIO(_asio->createBuffers(&result->buffers[0], static_cast<long>(buffers.size()), asioBufferSize, &result->callbacks));
  _streamOpen = true;
  *stream = result.release();
  return ASE_OK;
}

// ---- stream ----

XtFault AsioStream::Stop() {
  if(!XtiCompareExchange(running, 1, 0)) return ASE_OK;
  while(insideCallback.load() == 1);
  return device->_asio->stop();
}

XtFault AsioStream::Start() {
  XT_ASSERT(XtiCompareExchange(running, 0, 1));
  return device->_asio->start();
}

AsioStream::~AsioStream() { 
  XT_ASSERT(IsAsioSuccess(device->_asio->disposeBuffers())); 
  device->_streamOpen = false;
}

XtFault AsioStream::GetFrames(int32_t* frames) const {
  *frames = bufferSize;
  return ASE_OK;
}

XtFault AsioStream::GetLatency(XtLatency* latency) const {
  long input, output;
  ASIOSampleRate rate;
  XT_VERIFY_ASIO(device->_asio->getSampleRate(&rate));
  XT_VERIFY_ASIO(device->_asio->getLatencies(&input, &output));
  latency->input = _params.format.channels.inputs == 0? 0.0: input * 1000.0 / rate;
  latency->output = _params.format.channels.outputs == 0? 0.0: output * 1000.0 / rate;
  return ASE_OK;
}

#endif // XT_ENABLE_ASIO