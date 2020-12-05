// Don't warn about posix-compliant strdup() and friends.
#ifndef _CRT_NONSTDC_NO_DEPRECATE
#define _CRT_NONSTDC_NO_DEPRECATE 1
#endif

#include <xt/api/public/XtAudio.h>
#include <xt/api/public/XtDevice.h>
#include <xt/api/public/XtStream.h>
#include <xt/api/public/XtService.h>
#include <xt/api/private/Device.hpp>
#include <xt/api/private/Stream.hpp>
#include <xt/api/private/Service.hpp>
#include <xt/api/public/Shared.h>
#include <xt/api/public/Structs.h>
#include <xt/private/Shared.hpp>
#include <xt/private/Structs.hpp>
#include <xt/private/AggregateStream.hpp>
#include <sstream>
#include <cstring>
#include <iomanip>
#include <algorithm>
#include <inttypes.h>

// ---- local ----

static void InitStreamBuffers(
  XtIOBuffers& buffers, const XtFormat* format, int32_t frames) {

  int32_t sampleSize = XtiGetSampleSize(format->mix.sample);
  buffers.input.interleaved = std::vector<uint8_t>(frames * format->channels.inputs * sampleSize, 0);
  buffers.output.interleaved = std::vector<uint8_t>(frames * format->channels.outputs * sampleSize, 0);
  buffers.input.nonInterleaved = std::vector<void*>(format->channels.inputs, nullptr);
  buffers.output.nonInterleaved = std::vector<void*>(format->channels.outputs, nullptr);
  buffers.input.channels = std::vector<std::vector<uint8_t>>(
    format->channels.inputs, std::vector<uint8_t>(frames * sampleSize, 0));
  buffers.output.channels = std::vector<std::vector<uint8_t>>(
    format->channels.outputs, std::vector<uint8_t>(frames * sampleSize, 0));
  for(int32_t i = 0; i < format->channels.inputs; i++)
    buffers.input.nonInterleaved[i] = &(buffers.input.channels[i][0]);
  for(int32_t i = 0; i < format->channels.outputs; i++)
    buffers.output.nonInterleaved[i] = &(buffers.output.channels[i][0]);
}

static XtError OpenStreamInternal(XtDevice* d, const XtDeviceStreamParams* params, bool secondary, void* user, XtStream** stream) {

  XtError error;
  XtFault fault;
  int32_t frames;
  XtSystem system;
  XtBool supports;

  double rate = params->format.mix.rate;
  uint64_t inMask = params->format.channels.inMask;
  int32_t inputs = params->format.channels.inputs;
  uint64_t outMask = params->format.channels.outMask;
  int32_t outputs = params->format.channels.outputs;
  XtSample sample = params->format.mix.sample;

  auto attributes = XtAudioGetSampleAttributes(sample);

  *stream = nullptr;
  system = d->GetSystem();  
  if((error = XtDeviceSupportsAccess(d, params->stream.interleaved, &supports)) != 0)
    return error;
  if((fault = d->OpenStream(params, secondary, user, stream)) != 0)
    return XtiCreateError(d->GetSystem(), fault);
  if((fault = (*stream)->GetFrames(&frames)) != 0) {
    XtStreamDestroy(*stream);
    return XtiCreateError(d->GetSystem(), fault);
  }

  (*stream)->_user = user;
  (*stream)->_params = *params;
  (*stream)->_emulated = !supports;
  InitStreamBuffers((*stream)->_buffers, &params->format, frames);
  return 0;
}

// ---- service ----

XtError XT_CALL XtServiceAggregateStream(const XtService* s, const XtAggregateStreamParams* params, void* user, XtStream** stream) {

  XT_ASSERT(s != nullptr);
  XT_ASSERT(params != nullptr);
  XT_ASSERT(params->count > 0);
  XT_ASSERT(stream != nullptr);
  XT_ASSERT(XtiCalledOnMainThread());
  XT_ASSERT(params->master != nullptr);
  XT_ASSERT(params->devices != nullptr);
  XT_ASSERT(params->stream.onBuffer != nullptr);
  XT_ASSERT((s->GetCapabilities() & XtCapabilitiesAggregation) != 0);

  XtSystem system = s->GetSystem();
  auto attrs = XtAudioGetSampleAttributes(params->mix.sample);
  std::unique_ptr<XtAggregateStream> result(new XtAggregateStream);
  result->_user = user;
  result->_running = 0;
  result->_system = system;
  result->_masterIndex = -1;
  result->_insideCallback.store(0);
  result->_params.stream = params->stream;
  result->_emulated = false;
  result->_rings = std::vector<XtIORingBuffers>(params->count, XtIORingBuffers());
  result->_contexts = std::vector<XtAggregateContext>(params->count, XtAggregateContext());

  XtError error;
  int32_t frames = 0;
  int32_t thisFrames;
  XtStream* thisStream;
  XtFormat format = { 0 };
  bool masterFound = false;
  XtFormat thisFormat = { 0 };
  XtOnBuffer onThisBuffer;

  format.mix = params->mix;
  for(int32_t i = 0; i < params->count; i++) {
    thisFormat.mix = params->mix;
    thisFormat.channels.inputs = params->devices[i].channels.inputs;
    thisFormat.channels.inMask = params->devices[i].channels.inMask;
    thisFormat.channels.outputs = params->devices[i].channels.outputs;
    thisFormat.channels.outMask = params->devices[i].channels.outMask;

    result->_contexts[i].index = i;
    result->_contexts[i].stream = result.get();
    result->_channels.push_back(params->devices[i].channels);
    format.channels.inputs += params->devices[i].channels.inputs;
    format.channels.outputs += params->devices[i].channels.outputs;

    masterFound |= params->master == params->devices[i].device;
    if(params->master == params->devices[i].device)
      result->_masterIndex = i;
    onThisBuffer = params->master == params->devices[i].device? XtAggregateStream::OnMasterBuffer: XtAggregateStream::OnSlaveBuffer;
    XtDeviceStreamParams thisParams = { 0 };
    thisParams.bufferSize = params->devices[i].bufferSize;
    thisParams.format = thisFormat;
    thisParams.stream.interleaved = params->stream.interleaved;
    thisParams.stream.onBuffer = onThisBuffer;
    thisParams.stream.onXRun = params->stream.onXRun;
    if((error = OpenStreamInternal(params->devices[i].device, &thisParams, params->master != params->devices[i].device, &result->_contexts[i], &thisStream) != 0))
      return error;
    auto thisBlocking = &dynamic_cast<XtBlockingStream&>(*thisStream);
    result->_streams.push_back(std::unique_ptr<XtBlockingStream>(thisBlocking));
    thisBlocking->_index = i;
    thisBlocking->_aggregated = true;
    if((error = XtStreamGetFrames(thisStream, &thisFrames)) != 0)
      return error;
    frames = thisFrames > frames? thisFrames: frames;
  }
  XT_ASSERT(masterFound);

  result->_params.format = format;
  result->_frames = frames * 2;
  InitStreamBuffers(result->_weave, &format, frames);
  InitStreamBuffers(result->_buffers, &format, frames);
  for(int32_t i = 0; i < params->count; i++) {
    result->_rings[i].input = XtRingBuffer(params->stream.interleaved != XtFalse, result->_frames, params->devices[i].channels.inputs, attrs.size);
    result->_rings[i].output = XtRingBuffer(params->stream.interleaved != XtFalse, result->_frames, params->devices[i].channels.outputs, attrs.size);
  }

  *stream = result.release();
  return 0;
}

// ---- device ----

XtError XT_CALL XtDeviceOpenStream(XtDevice* d, const XtDeviceStreamParams* params, void* user, XtStream** stream) {  

  XT_ASSERT(d != nullptr);
  XT_ASSERT(params != nullptr);
  XT_ASSERT(stream != nullptr);
  XT_ASSERT(XtiCalledOnMainThread());
  XT_ASSERT(params->bufferSize > 0.0);
  XT_ASSERT(params->stream.onBuffer != nullptr);
  return OpenStreamInternal(d, params, false, user, stream);
}