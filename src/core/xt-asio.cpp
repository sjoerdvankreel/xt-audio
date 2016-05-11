#ifdef _WIN32
#include "xt-win32.hpp"
#include <asmjit.h>
#include <common/iasiodrv.h>
#include <host/pc/asiolist.h>
#include <memory>
#include <vector>

/* Copyright (C) 2015-2016 Sjoerd van Kreel.
 *
 * This file is part of XT-Audio.
 *
 * XT-Audio is free software: you can redistribute it and/or modify it under the 
 * terms of the GNU Lesser General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later version.
 *
 * XT-Audio is distributed in the hope that it will be useful, but WITHOUT ANY 
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with XT-Audio. If not, see<http://www.gnu.org/licenses/>.
 */

// ---- local ----

static bool IsAsioSuccess(ASIOError e);
static const double XtAsioNsPerMs = 1000000.0;
static const int ASE_Format = ASE_NoMemory + 1;
static size_t GetFrameSize(const XtFormat& format, bool output);

#define XT_ASIO_CALL __cdecl
#define XT_TO_UINT64(lo, hi) ((uint64_t)(lo) | ((uint64_t)(hi) << 32))
#define XT_VERIFY_ASIO(c) do { auto e = (c); \
  if(!IsAsioSuccess(e)) return XT_TRACE(XtLevelError, #c), e; } while(0)

typedef void (XT_ASIO_CALL* SdkBufferSwitch)(long, ASIOBool);
typedef void (XT_ASIO_CALL* ContextBufferSwitch)(void*, long, ASIOBool);
typedef ASIOTime* (XT_ASIO_CALL* SdkBufferSwitchTimeInfo)(ASIOTime*, long, ASIOBool);
typedef ASIOTime* (XT_ASIO_CALL* ContextBufferSwitchTimeInfo)(void*, ASIOTime*, long, ASIOBool);

// ---- forward ----

XT_DECLARE_SERVICE(Asio);

struct AsioDevice: public XtDevice {
  bool streamOpen;
  const std::string name;
  const CComPtr<IASIO> asio;
  XT_IMPLEMENT_DEVICE(Asio);
  
  ~AsioDevice() { XT_ASSERT(!streamOpen); }
  AsioDevice(const std::string& name, CComPtr<IASIO> asio): 
  XtDevice(), streamOpen(false), name(name), asio(asio) {}
};

struct AsioStream: public XtStream {
  bool issueOutputReady;
  const long bufferSize;
  AsioDevice* const device;
  std::vector<ASIOBufferInfo> buffers;
  std::vector<char> inputInterleaved;
  std::vector<char> outputInterleaved;
  const std::unique_ptr<asmjit::JitRuntime> runtime;
  XT_IMPLEMENT_STREAM(Asio);
  XT_IMPLEMENT_STREAM_CONTROL();

  ~AsioStream();
  AsioStream(AsioDevice* d, const XtFormat& format, 
    size_t bufferSize, const std::vector<ASIOBufferInfo>& buffers):
  XtStream(), issueOutputReady(true), 
  bufferSize(static_cast<long>(bufferSize)), device(d), buffers(buffers),
  inputInterleaved(GetFrameSize(format, false) * bufferSize, '\0'),
  outputInterleaved(GetFrameSize(format, true) * bufferSize, '\0'),
  runtime(std::make_unique<asmjit::JitRuntime>()) {}
};

// ---- local ----

static bool IsAsioSuccess(ASIOError e) {
  return e == ASE_OK || e == ASE_SUCCESS; 
}

static size_t GetFrameSize(const XtFormat& format, bool output) {
  XtAttributes attrs;
  XtAudioGetSampleAttributes(format.mix.sample, &attrs);
  return (output? format.outputs: format.inputs) * attrs.size;
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
  void* channel;
  double time = 0.0;
  uint64_t position = 0;
  XtBool timeValid = XtFalse;
  AsioTimeInfo& info = asioTime->timeInfo;
  AsioStream* s = static_cast<AsioStream*>(ctx);
  int32_t inputs = s->format.inputs;
  int32_t outputs = s->format.outputs;
  int32_t sampleSize = XtiGetSampleSize(s->format.mix.sample);

  if(info.flags & kSamplePositionValid && info.flags & kSystemTimeValid) {
    timeValid = XtTrue;
    position = XT_TO_UINT64(info.samplePosition.lo, info.samplePosition.hi);
    time = XT_TO_UINT64(info.systemTime.lo, info.systemTime.hi) / XtAsioNsPerMs;
  }

  input = inputs > 0? &s->inputInterleaved[0]: nullptr;
  output = outputs > 0? &s->outputInterleaved[0]: nullptr;
  for(int32_t i = 0; i < inputs; i++) {
    channel = s->buffers[i].buffers[index];
    XtiInterleave(&s->inputInterleaved[0], channel, s->bufferSize, inputs, sampleSize, i);
  }
  s->callback(s, input, output, s->bufferSize, time, position, timeValid, ASE_OK, s->user);
  for(int32_t o = 0; o < outputs; o++) {
    channel = s->buffers[inputs + o].buffers[index];
    XtiDeinterleave(channel, &s->outputInterleaved[0], s->bufferSize, outputs, sampleSize, o);
  }

  if(s->issueOutputReady)
    s->issueOutputReady = s->device->asio->outputReady() == ASE_OK;
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
  X86Assembler assembler(runtime);
  X86Compiler compiler(&assembler);
  
  auto sdkProto = FuncBuilder2<void, long, ASIOBool>(kCallConvHostCDecl);
  X86FuncNode* function = compiler.addFunc(sdkProto);
  X86GpVar index = compiler.newInt32("index");
  X86GpVar directProcess = compiler.newInt32("directProcess");
  compiler.setArg(0, index);
  compiler.setArg(1, directProcess);

  auto ctxProto = FuncBuilder3<void, void*, long, ASIOBool>(kCallConvHostCDecl);
  X86CallNode* call = compiler.call(asmjit_cast<Ptr>(target), ctxProto);
  call->setArg(0, imm_ptr(ctx));
  call->setArg(1, index);
  call->setArg(2, directProcess);
  
  compiler.endFunc();
  compiler.finalize();
  return asmjit_cast<SdkBufferSwitch>(assembler.make());
}

static SdkBufferSwitchTimeInfo JitBufferSwitchTimeInfo(
  asmjit::JitRuntime* runtime, ContextBufferSwitchTimeInfo target, void* ctx) {

  using namespace asmjit;
  X86Assembler assembler(runtime);
  X86Compiler compiler(&assembler);
  
  auto sdkProto = FuncBuilder3<ASIOTime*, ASIOTime*, long, ASIOBool>(kCallConvHostCDecl);
  X86FuncNode* function = compiler.addFunc(sdkProto);
  X86GpVar params = compiler.newIntPtr("params");
  X86GpVar index = compiler.newInt32("index");
  X86GpVar directProcess = compiler.newInt32("directProcess");
  compiler.setArg(0, params);
  compiler.setArg(1, index);
  compiler.setArg(2, directProcess);

  auto ctxProto = FuncBuilder4<ASIOTime*, void*, ASIOTime*, long, ASIOBool>(kCallConvHostCDecl);
  X86CallNode* call = compiler.call(asmjit_cast<Ptr>(target), ctxProto);
  call->setArg(0, imm_ptr(ctx));
  call->setArg(1, params);
  call->setArg(2, index);
  call->setArg(3, directProcess);

  X86GpVar ret = compiler.newIntPtr("ret");
  call->setRet(0, ret);
  compiler.ret(ret);
  compiler.endFunc();
  compiler.finalize();
  return asmjit_cast<SdkBufferSwitchTimeInfo>(assembler.make());
}

// ---- service ----

const char* AsioService::GetName() const {
  return "ASIO";
}

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
  return error == 0? ASE_OK: XtErrorGetFault(error);
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

XtFault AsioDevice::GetName(char** name) const {
  *name = _strdup(this->name.c_str());
  return ASE_OK;
}

XtFault AsioDevice::GetMix(XtMix** mix) const {

  XtSample sample;
  ASIOSampleType type;
  ASIOSampleRate rate;
  bool typeFixed = false;
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

  *mix = static_cast<XtMix*>(malloc(sizeof(XtMix)));
  (*mix)->sample = sample;
  (*mix)->rate = static_cast<int32_t>(rate);
  return ASE_OK;
}

XtFault AsioDevice::GetChannelCount(XtBool output, int32_t* count) const {
  long inputs, outputs;
  XT_VERIFY_ASIO(asio->getChannels(&inputs, &outputs));
  *count = output? outputs: inputs;
  return ASE_OK;
}

XtFault AsioDevice::GetBuffer(const XtFormat* format, XtBuffer* buffer) const {  
  ASIOSampleRate rate;
  long min, max, preferred, granularity;
  XT_VERIFY_ASIO(asio->getSampleRate(&rate));
  XT_VERIFY_ASIO(asio->getBufferSize(&min, &max, &preferred, &granularity));
  buffer->min = min * 1000.0 / rate;
  buffer->max = max * 1000.0 / rate;
  buffer->current = preferred * 1000.0 / rate;
  return ASE_OK;
}

XtFault AsioDevice::GetChannelName(XtBool output, int32_t index, char** name) const {
  long inputs, outputs;
  ASIOChannelInfo info = { 0 };
  XT_VERIFY_ASIO(asio->getChannels(&inputs, &outputs));
  info.isInput = !output;
  info.channel = index;
  XT_VERIFY_ASIO(asio->getChannelInfo(&info));
  *name = _strdup(info.name);
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
  if(format->inputs > inputs || format->outputs > outputs)
    return ASE_OK;
  for(int32_t i = inputs; i < 64; i++)
    if((format->inMask & (1ULL << i)) != 0)
      return ASE_OK;
  for(int32_t i = outputs; i < 64; i++)
    if((format->outMask & (1ULL << i)) != 0)
      return ASE_OK;
  
  for(size_t i = 0; i < infos.size(); i++) {
    if(infos[i].isInput && infos[i].type != type &&
      (format->inMask == 0 && infos[i].channel < format->inputs 
        || format->inMask != 0 && (((format->inMask >> infos[i].channel) & 1ULL) == 1ULL)))
      return ASE_OK;
    if(!infos[i].isInput && infos[i].type != type &&
      (format->outMask == 0 && infos[i].channel < format->outputs 
        || format->outMask != 0 && (((format->outMask >> infos[i].channel) & 1ULL) == 1ULL)))
      return ASE_OK;
  }

  *supports = XtTrue;
  return ASE_OK;
}

XtFault AsioDevice::OpenStream(const XtFormat* format, double bufferSize, XtStreamCallback callback, void* user, XtStream** stream) {
  
  double wantedSize;
  long asioBufferSize;
  ASIOSampleRate rate;
  ASIOCallbacks callbacks;
  std::vector<ASIOBufferInfo> buffers;
  long min, max, preferred, granularity;

  if(streamOpen)
    return DRVERR_DEVICE_ALREADY_OPEN;
  XT_VERIFY_ASIO(asio->getSampleRate(&rate));
  XT_VERIFY_ASIO(asio->getBufferSize(&min, &max, &preferred, &granularity));

  wantedSize = bufferSize / 1000.0 * rate;
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

  CreateBufferInfos(buffers, ASIOTrue, format->inputs, format->inMask);
  CreateBufferInfos(buffers, ASIOFalse, format->outputs, format->outMask);
  auto result = std::make_unique<AsioStream>(this, *format, asioBufferSize, buffers);
  callbacks.asioMessage = &AsioMessage;
  callbacks.sampleRateDidChange = &SampleRateDidChange;
  callbacks.bufferSwitch = JitBufferSwitch(result->runtime.get(), &BufferSwitch, result.get());
  callbacks.bufferSwitchTimeInfo = JitBufferSwitchTimeInfo(result->runtime.get(), &BufferSwitchTimeInfo, result.get());
  XT_VERIFY_ASIO(asio->createBuffers(&result->buffers[0], static_cast<long>(buffers.size()), asioBufferSize, &callbacks));
  streamOpen = true;
  *stream = result.release();
  return ASE_OK;
}

// ---- stream ----

XtFault AsioStream::Stop() {
  return device->asio->stop();
}

XtFault AsioStream::Start() {
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
  latency->input = format.inputs == 0? 0.0: input * 1000.0 / rate;
  latency->output = format.outputs == 0? 0.0: output * 1000.0 / rate;
  return ASE_OK;
}

#endif // _WIN32
