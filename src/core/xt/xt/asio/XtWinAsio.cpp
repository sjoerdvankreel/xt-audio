#if XT_ENABLE_ASIO
#include <xt/asio/Shared.hpp>
#include <memory>
#include <vector>
#include <atomic>




// ---- forward ----

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

#endif // XT_ENABLE_ASIO