// Don't warn about posix-compliant strdup() and friends.
#ifndef _CRT_NONSTDC_NO_DEPRECATE
#define _CRT_NONSTDC_NO_DEPRECATE 1
#endif

#include <xt/Private.hpp>
#include <sstream>
#include <cstring>
#include <iomanip>
#include <algorithm>
#include <inttypes.h>

// ---- local ----

static void InitStreamBuffers(
  XtIntermediateBuffers& buffers, XtBool interleaved, XtBool nonInterleaved,
  const XtFormat* format, int32_t frames, int32_t sampleSize) {

  if(interleaved) {
    buffers.inputInterleaved = std::vector<char>(frames * format->channels.inputs * sampleSize, '\0');
    buffers.outputInterleaved = std::vector<char>(frames * format->channels.outputs * sampleSize, '\0');
  }

  if(nonInterleaved) {
    buffers.inputNonInterleaved = std::vector<void*>(format->channels.inputs, nullptr);
    buffers.outputNonInterleaved = std::vector<void*>(format->channels.outputs, nullptr);
    buffers.inputChannelsNonInterleaved = std::vector<std::vector<char>>(
      format->channels.inputs, std::vector<char>(frames * sampleSize, '\0'));
    buffers.outputChannelsNonInterleaved = std::vector<std::vector<char>>(
      format->channels.outputs, std::vector<char>(frames * sampleSize, '\0'));
    for(int32_t i = 0; i < format->channels.inputs; i++)
      buffers.inputNonInterleaved[i] = &(buffers.inputChannelsNonInterleaved[i][0]);
    for(int32_t i = 0; i < format->channels.outputs; i++)
      buffers.outputNonInterleaved[i] = &(buffers.outputChannelsNonInterleaved[i][0]);
  }
}

static XtError OpenStreamInternal(XtDevice* d, const XtDeviceStreamParams* params, bool secondary, void* user, XtStream** stream) {

  XtError error;
  XtFault fault;
  int32_t frames;
  XtSystem system;
  XtBool supports;

  XT_ASSERT(d != nullptr);
  XT_ASSERT(params != nullptr);
  XT_ASSERT(stream != nullptr);
  XT_ASSERT(XtiCalledOnMainThread());
  XT_ASSERT(params->bufferSize > 0.0);
  XT_ASSERT(params->stream.onBuffer != nullptr);

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

  (*stream)->user = user;
  (*stream)->_params = *params;
  (*stream)->aggregated = false;
  (*stream)->aggregationIndex = 0;
  (*stream)->_emulated = !supports;
  XtBool initInterleaved = params->stream.interleaved && !supports;
  XtBool initNonInterleaved = !params->stream.interleaved && !supports;
  InitStreamBuffers((*stream)->intermediate, initInterleaved, initNonInterleaved, &params->format, frames, attributes.size);
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

  XtSystem system = s->GetSystem();
  auto attrs = XtAudioGetSampleAttributes(params->mix.sample);
  std::unique_ptr<XtAggregate> result(new XtAggregate);
  result->user = user;
  result->running = 0;
  result->system = system;
  result->masterIndex = -1;
  result->aggregated = false;
  result->aggregationIndex = -1;
  result->insideCallbackCount = 0;
  result->_params.stream = params->stream;
  result->_emulated = false;
  result->inputRings = std::vector<XtRingBuffer>(params->count, XtRingBuffer());
  result->outputRings = std::vector<XtRingBuffer>(params->count, XtRingBuffer());
  result->contexts = std::vector<XtAggregateContext>(params->count, XtAggregateContext());

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

    result->contexts[i].index = i;
    result->contexts[i].stream = result.get();
    result->channels.push_back(params->devices[i].channels);
    format.channels.inputs += params->devices[i].channels.inputs;
    format.channels.outputs += params->devices[i].channels.outputs;

    masterFound |= params->master == params->devices[i].device;
    if(params->master == params->devices[i].device)
      result->masterIndex = i;
    onThisBuffer = params->master == params->devices[i].device? XtiOnMasterBuffer: XtiOnSlaveBuffer;
    XtDeviceStreamParams thisParams = { 0 };
    thisParams.bufferSize = params->devices[i].bufferSize;
    thisParams.format = thisFormat;
    thisParams.stream.interleaved = params->stream.interleaved;
    thisParams.stream.onBuffer = onThisBuffer;
    thisParams.stream.onXRun = params->stream.onXRun;
    if((error = OpenStreamInternal(params->devices[i].device, &thisParams, params->master != params->devices[i].device, &result->contexts[i], &thisStream) != 0))
      return error;
    result->streams.push_back(std::unique_ptr<XtStream>(thisStream));
    thisStream->aggregated = true;
    thisStream->aggregationIndex = i;
    if((error = XtStreamGetFrames(thisStream, &thisFrames)) != 0)
      return error;
    frames = thisFrames > frames? thisFrames: frames;
  }
  XT_ASSERT(masterFound);

  result->_params.format = format;
  result->frames = frames * 2;
  InitStreamBuffers(result->weave, params->stream.interleaved, !params->stream.interleaved, &format, frames, attrs.size);
  InitStreamBuffers(result->intermediate, params->stream.interleaved, !params->stream.interleaved, &format, frames, attrs.size);
  for(int32_t i = 0; i < params->count; i++) {
    result->inputRings[i] = XtRingBuffer(params->stream.interleaved != XtFalse, result->frames, params->devices[i].channels.inputs, attrs.size);
    result->outputRings[i] = XtRingBuffer(params->stream.interleaved != XtFalse, result->frames, params->devices[i].channels.outputs, attrs.size);
  }

  *stream = result.release();
  return 0;
}

// ---- device ----

XtError XT_CALL XtDeviceOpenStream(XtDevice* d, const XtDeviceStreamParams* params, void* user, XtStream** stream) {

  return OpenStreamInternal(d, params, false, user, stream);
}