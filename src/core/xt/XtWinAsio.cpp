#ifdef _WIN32

// Windows.h's min/max collide with asmjit.
#ifndef NOMINMAX
#define NOMINMAX 1 
#endif // NOMINMAX
#include <xt/XtWin32.hpp>

#ifdef XT_DISABLE_ASIO
const XtService* XtiServiceAsio = nullptr;
#else // XT_DISABLE_ASIO

#include <asmjit/asmjit.h>
#include <common/iasiodrv.h>
#include <host/pc/asiolist.h>
#include <memory>
#include <vector>

// ---- local ----

static bool IsAsioSuccess(ASIOError e);
static const double XtAsioNsPerMs = 1000000.0;
static const int ASE_Format = ASE_NoMemory + 1;

#define XT_ASIO_CALL __cdecl
#define XT_TO_UINT64(lo, hi) ((uint64_t)(lo) | ((uint64_t)(hi) << 32))
#define XT_VERIFY_ASIO(c) do { auto e = (c); \
  if(!IsAsioSuccess(e)) return XT_TRACE(#c), e; } while(0)

typedef void (XT_ASIO_CALL* SdkBufferSwitch)(long, ASIOBool);
typedef void (XT_ASIO_CALL* ContextBufferSwitch)(void*, long, ASIOBool);
typedef ASIOTime* (XT_ASIO_CALL* SdkBufferSwitchTimeInfo)(ASIOTime*, long, ASIOBool);
typedef ASIOTime* (XT_ASIO_CALL* ContextBufferSwitchTimeInfo)(void*, ASIOTime*, long, ASIOBool);

// ---- forward ----

XT_DECLARE_SERVICE(ASIO, Asio);

struct AsioDevice: public XtDevice {
  bool streamOpen;
  const std::string name;
  const CComPtr<IASIO> asio;
  XT_IMPLEMENT_DEVICE(ASIO);
  
  ~AsioDevice() { XT_ASSERT(!streamOpen); }
  AsioDevice(const std::string& name, CComPtr<IASIO> asio): 
  XtDevice(), streamOpen(false), name(name), asio(asio) {}
};

struct AsioStream: public XtStream {
  bool issueOutputReady;
  const long bufferSize;
  ASIOCallbacks callbacks;
  AsioDevice* const device;
  volatile int32_t running;
  volatile int32_t insideCallback;
  std::vector<void*> inputChannels;
  std::vector<void*> outputChannels;
  std::vector<ASIOBufferInfo> buffers;
  const std::unique_ptr<asmjit::JitRuntime> runtime;
  XT_IMPLEMENT_CALLBACK_STREAM(ASIO);

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

static bool IsAsioSuccess(ASIOError e) {
  return e == ASE_OK || e == ASE_SUCCESS; 
}

static bool ToAsioSample(XtSample sample, ASIOSampleType& asio) {
  switch(sample) {
  case XtSampleInt16: asio = ASIOSTInt16LSB; return true;
  case XtSampleInt24: asio = ASIOSTInt24LSB; return true;
  case XtSampleInt32: asio = ASIOSTInt32LSB; return true;
  case XtSampleFloat32: asio = ASIOSTFloat32LSB; return true;
  default: return false;
  }
}

static bool FromAsioSample(ASIOSampleType asio, XtSample& sample) {
  switch(asio) {
  case ASIOSTInt16LSB: sample = XtSampleInt16; return true;
  case ASIOSTInt24LSB: sample = XtSampleInt24; return true;
  case ASIOSTInt32LSB: sample = XtSampleInt32; return true;
  case ASIOSTFloat32LSB: sample = XtSampleFloat32; return true;
  default: return false;
  }
}

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

static ASIOError GetChannelInfos(
  IASIO* asio, std::vector<ASIOChannelInfo>& infos, ASIOBool input, long channels) {

  for(long i = 0; i < channels; i++) {
    ASIOChannelInfo info = { 0 };
    info.isInput = input;
    info.channel = i;
    XT_VERIFY_ASIO(asio->getChannelInfo(&info));
    infos.push_back(info);
  }
  return ASE_OK;
}

static ASIOError GetChannelInfos(IASIO* asio, std::vector<ASIOChannelInfo>& infos) {
  long inputs, outputs;
  XT_VERIFY_ASIO(asio->getChannels(&inputs, &outputs));
  XT_VERIFY_ASIO(GetChannelInfos(asio, infos, ASIOTrue, inputs));
  XT_VERIFY_ASIO(GetChannelInfos(asio, infos, ASIOFalse, outputs));
  return ASE_OK;
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

  if(XtiCas(&s->running, 1, 1) != 1)
    return nullptr;
  if(XtiCas(&s->insideCallback, 1, 0) != 0)
    return nullptr;

  if(info.flags & kSamplePositionValid && info.flags & kSystemTimeValid) {
    timeValid = XtTrue;
    position = XT_TO_UINT64(info.samplePosition.lo, info.samplePosition.hi);
    time = XT_TO_UINT64(info.systemTime.lo, info.systemTime.hi) / XtAsioNsPerMs;
  }

  input = s->format.channels.inputs > 0? &s->inputChannels[0]: nullptr;
  output = s->format.channels.outputs > 0? &s->outputChannels[0]: nullptr;
  for(int32_t i = 0; i < s->format.channels.inputs; i++)
    s->inputChannels[i] = s->buffers[i].buffers[index];
  for(int32_t i = 0; i < s->format.channels.outputs; i++)
    s->outputChannels[i] = s->buffers[s->format.channels.inputs + i].buffers[index];

  XtBuffer buffer = { 0 };
  buffer.input = input;
  buffer.output = output;
  buffer.frames = s->bufferSize;
  buffer.time = time;
  buffer.position = position;
  buffer.timeValid = timeValid;
  s->OnBuffer(&buffer);
  if(s->issueOutputReady)
    s->issueOutputReady = s->device->asio->outputReady() == ASE_OK;

  XT_ASSERT(XtiCas(&s->insideCallback, 0, 1) == 1);
  return nullptr; 
}

static void XT_ASIO_CALL BufferSwitch(void* ctx, long index, ASIOBool direct) {

  ASIOTime time = { 0 };
  AsioTimeInfo& info = time.timeInfo;
  AsioStream* stream = static_cast<AsioStream*>(ctx);
	if(stream->device->asio->getSamplePosition(&info.samplePosition, &info.systemTime) == ASE_OK)
		info.flags = kSystemTimeValid | kSamplePositionValid;
	BufferSwitchTimeInfo(ctx, &time, index, direct);
}

// ---- local ----

static SdkBufferSwitch JitBufferSwitch(
  asmjit::JitRuntime* runtime, ContextBufferSwitch target, void* ctx) {

  using namespace asmjit;
  CodeHolder code;
  code.init(runtime->codeInfo());
  x86::Compiler compiler(&code);
  
  auto sdkProto = FuncSignatureT<void, long, ASIOBool>(CallConv::kIdHostCDecl);
  FuncNode* function = compiler.addFunc(sdkProto);
  x86::Gp index = compiler.newInt32("index");
  x86::Gp directProcess = compiler.newInt32("directProcess");
  compiler.setArg(0, index);
  compiler.setArg(1, directProcess);

  auto ctxProto = FuncSignatureT<void, void*, long, ASIOBool>(CallConv::kIdHostCDecl);
  FuncCallNode* call = compiler.call(imm(target), ctxProto);
  call->setArg(0, imm(ctx));
  call->setArg(1, index);
  call->setArg(2, directProcess);
  
  compiler.endFunc();
  compiler.finalize();

  SdkBufferSwitch result;
  XT_ASSERT(!runtime->add(&result, &code));
  return result;
}

static SdkBufferSwitchTimeInfo JitBufferSwitchTimeInfo(
  asmjit::JitRuntime* runtime, ContextBufferSwitchTimeInfo target, void* ctx) {

  using namespace asmjit;
  CodeHolder code;
  code.init(runtime->codeInfo());
  x86::Compiler compiler(&code);
  
  auto sdkProto = FuncSignatureT<ASIOTime*, ASIOTime*, long, ASIOBool>(CallConv::kIdHostCDecl);
  FuncNode* function = compiler.addFunc(sdkProto);
  x86::Gp params = compiler.newIntPtr("params");
  x86::Gp index = compiler.newInt32("index");
  x86::Gp directProcess = compiler.newInt32("directProcess");
  compiler.setArg(0, params);
  compiler.setArg(1, index);
  compiler.setArg(2, directProcess);

  auto ctxProto = FuncSignatureT<ASIOTime*, void*, ASIOTime*, long, ASIOBool>(CallConv::kIdHostCDecl);
  FuncCallNode* call = compiler.call(imm(target), ctxProto);
  call->setArg(0, imm(ctx));
  call->setArg(1, params);
  call->setArg(2, index);
  call->setArg(3, directProcess);

  x86::Gp ret = compiler.newIntPtr("ret");
  call->setRet(0, ret);
  compiler.ret(ret);

  compiler.endFunc();
  compiler.finalize();

  SdkBufferSwitchTimeInfo result;
  XT_ASSERT(!runtime->add(&result, &code));
  return result;
}

// ---- service ----

XtFault AsioService::GetFormatFault() const {
  return ASE_Format;
}

XtCapabilities AsioService::GetCapabilities() const {
  return static_cast<XtCapabilities>(
    XtCapabilitiesTime | 
    XtCapabilitiesLatency | 
    XtCapabilitiesFullDuplex | 
    XtCapabilitiesChannelMask);
}

XtFault AsioService::GetDeviceCount(int32_t* count) const {
  *count = AsioDriverList().asioGetNumDev();
  return ASE_OK;
}

XtFault AsioService::OpenDefaultDevice(XtBool output, XtDevice** device) const  { 
  XtError error;
  if(AsioDriverList().asioGetNumDev() == 0)
    return ASE_OK;
  error = XtServiceOpenDevice(this, 0, device);
  return XtiGetErrorFault(error);
}

XtCause AsioService::GetFaultCause(XtFault fault) const {
  switch(fault) {
  case ASE_Format: return XtCauseFormat;
  case DRVERR_DEVICE_NOT_FOUND:
  case DRVERR_DEVICE_ALREADY_OPEN:
  case ASE_NotPresent:
  case ASE_NoClock: return XtCauseEndpoint;
  default: return XtCauseUnknown;
  }
}

const char* AsioService::GetFaultText(XtFault fault) const {
  switch(fault) {
  case ASE_Format: return "ASE_Format";
  case ASE_NoClock: return "ASE_NoClock";
  case ASE_NoMemory: return "ASE_NoMemory";
  case ASE_NotPresent: return "ASE_NotPresent";
  case ASE_InvalidMode: return "ASE_InvalidMode";
  case ASE_HWMalfunction: return "ASE_HWMalfunction";
  case ASE_SPNotAdvancing: return "ASE_SPNotAdvancing";
  case ASE_InvalidParameter: return "ASE_InvalidParameter";
  case DRVERR_INVALID_PARAM: return "DRVERR_INVALID_PARAM";
  case DRVERR_DEVICE_NOT_FOUND: return "DRVERR_DEVICE_NOT_FOUND";
  case DRVERR_DEVICE_ALREADY_OPEN: return "DRVERR_DEVICE_ALREADY_OPEN";
  default: return "Unknown fault.";
  }
}

XtFault AsioService::OpenDevice(int32_t index, XtDevice** device) const  {  

  HRESULT hr;
  CLSID classId;
  CComPtr<IASIO> asio;
  AsioDriverList list;
  std::string name(MAXDRVNAMELEN, '\0');

  XT_VERIFY_ASIO(list.asioGetDriverName(index, &name[0], MAXDRVNAMELEN));
  XT_VERIFY_ASIO(list.asioGetDriverCLSID(index, &classId));
  XT_VERIFY_COM(CoCreateInstance(classId, nullptr, CLSCTX_ALL, classId, reinterpret_cast<void**>(&asio)));
  if(!asio->init(XtwGetWindow()))
    return ASE_NotPresent;
  *device = new AsioDevice(name, asio);
  return ASE_OK;
}

// ---- device ----

XtFault AsioDevice::ShowControlPanel() {
  return asio->controlPanel();
}

XtFault AsioDevice::GetName(char* buffer, int32_t* size) const {
  XtiOutputString(this->name.c_str(), buffer, size);
  return ASE_OK;
}

XtFault AsioDevice::SupportsAccess(XtBool interleaved, XtBool* supports) const {
  *supports = !interleaved;
  return ASE_OK;
}

XtFault AsioDevice::GetMix(XtBool* valid, XtMix* mix) const {

  XtSample sample;
  ASIOSampleRate rate;
  bool typeFixed = false;
  ASIOSampleType type = -1;
  std::vector<ASIOChannelInfo> infos;
  ASIOError error = asio->getSampleRate(&rate);

  if(error == ASE_NoClock || error == ASE_NotPresent)
    return ASE_OK;
  if(error != ASE_OK)
    return error;

  XT_VERIFY_ASIO(GetChannelInfos(asio, infos));
  for(size_t i = 0; i < infos.size(); i++) {
      if(typeFixed && type != infos[i].type)
        return ASE_Format;
      else if(!typeFixed) {
        typeFixed = true;
        type = infos[i].type;
      }
  }

  if(!FromAsioSample(type, sample))
    return ASE_Format;

  *valid = XtTrue;
  mix->sample = sample;
  mix->rate = static_cast<int32_t>(rate);
  return ASE_OK;
}

XtFault AsioDevice::GetChannelCount(XtBool output, int32_t* count) const {
  long inputs, outputs;
  XT_VERIFY_ASIO(asio->getChannels(&inputs, &outputs));
  *count = output? outputs: inputs;
  return ASE_OK;
}

XtFault AsioDevice::GetBufferSize(const XtFormat* format, XtBufferSize* size) const {  
  ASIOSampleRate rate;
  long min, max, preferred, granularity;
  XT_VERIFY_ASIO(asio->getSampleRate(&rate));
  XT_VERIFY_ASIO(asio->getBufferSize(&min, &max, &preferred, &granularity));
  size->min = min * 1000.0 / rate;
  size->max = max * 1000.0 / rate;
  size->current = preferred * 1000.0 / rate;
  return ASE_OK;
}

XtFault AsioDevice::GetChannelName(XtBool output, int32_t index, char* buffer, int32_t* size) const {
  long inputs, outputs;
  ASIOChannelInfo info = { 0 };
  XT_VERIFY_ASIO(asio->getChannels(&inputs, &outputs));
  info.isInput = !output;
  info.channel = index;
  XT_VERIFY_ASIO(asio->getChannelInfo(&info));
  XtiOutputString(info.name, buffer, size);
  return ASE_OK;
}

XtFault AsioDevice::SupportsFormat(const XtFormat* format, XtBool* supports) const {
  
  ASIOSampleType type;
  ASIOSampleRate rate;
  long inputs, outputs;
  std::vector<ASIOChannelInfo> infos;
  
  XT_VERIFY_ASIO(asio->getSampleRate(&rate));
  XT_VERIFY_ASIO(GetChannelInfos(asio, infos));
  XT_VERIFY_ASIO(asio->getChannels(&inputs, &outputs));
  
  if(!ToAsioSample(format->mix.sample, type))
    return ASE_OK;
  if(static_cast<int32_t>(rate) != format->mix.rate)
    return ASE_OK;
  if(format->channels.inputs > inputs || format->channels.outputs > outputs)
    return ASE_OK;
  for(int32_t i = inputs; i < 64; i++)
    if((format->channels.inMask & (1ULL << i)) != 0)
      return ASE_OK;
  for(int32_t i = outputs; i < 64; i++)
    if((format->channels.outMask & (1ULL << i)) != 0)
      return ASE_OK;
  
  for(size_t i = 0; i < infos.size(); i++) {
    if(infos[i].isInput && infos[i].type != type &&
      (format->channels.inMask == 0 && infos[i].channel < format->channels.inputs 
        || format->channels.inMask != 0 && (((format->channels.inMask >> infos[i].channel) & 1ULL) == 1ULL)))
      return ASE_OK;
    if(!infos[i].isInput && infos[i].type != type &&
      (format->channels.outMask == 0 && infos[i].channel < format->channels.outputs 
        || format->channels.outMask != 0 && (((format->channels.outMask >> infos[i].channel) & 1ULL) == 1ULL)))
      return ASE_OK;
  }

  *supports = XtTrue;
  return ASE_OK;
}

XtFault AsioDevice::OpenStream(const XtDeviceStreamParams* params, bool secondary, void* user, XtStream** stream) {
  
  double wantedSize;
  long asioBufferSize;
  ASIOSampleRate rate;
  std::vector<ASIOBufferInfo> buffers;
  long min, max, preferred, granularity;

  if(streamOpen)
    return static_cast<XtFault>(DRVERR_DEVICE_ALREADY_OPEN);
  XT_VERIFY_ASIO(asio->getSampleRate(&rate));
  XT_VERIFY_ASIO(asio->getBufferSize(&min, &max, &preferred, &granularity));

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
  XT_VERIFY_ASIO(asio->createBuffers(&result->buffers[0], static_cast<long>(buffers.size()), asioBufferSize, &result->callbacks));
  streamOpen = true;
  *stream = result.release();
  return ASE_OK;
}

// ---- stream ----

XtFault AsioStream::Stop() {
  XtiCas(&running, 0, 1);
  while(XtiCas(&insideCallback, 1, 1) == 1);
  return device->asio->stop();
}

XtFault AsioStream::Start() {
  XtiCas(&running, 1, 0);
  return device->asio->start();
}

AsioStream::~AsioStream() { 
  XT_ASSERT(IsAsioSuccess(device->asio->disposeBuffers())); 
  device->streamOpen = false;
}

XtFault AsioStream::GetFrames(int32_t* frames) const {
  *frames = bufferSize;
  return ASE_OK;
}

XtFault AsioStream::GetLatency(XtLatency* latency) const {
  long input, output;
  ASIOSampleRate rate;
  XT_VERIFY_ASIO(device->asio->getSampleRate(&rate));
  XT_VERIFY_ASIO(device->asio->getLatencies(&input, &output));
  latency->input = format.channels.inputs == 0? 0.0: input * 1000.0 / rate;
  latency->output = format.channels.outputs == 0? 0.0: output * 1000.0 / rate;
  return ASE_OK;
}

#endif // XT_DISABLE_ASIO
#endif // _WIN32