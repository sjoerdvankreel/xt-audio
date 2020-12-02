#include <xt/Private.hpp>
#include <xt/api/private/Platform.hpp>
#include <cassert>
#include <atomic>

// ---- local ----

static void Weave(
  void* dest, const void* source, XtBool interleaved,
  int32_t destChannels, int32_t sourceChannels, 
  int32_t destChannel, int32_t sourceChannel, 
  int32_t frames, int32_t sampleSize) {

  assert(destChannels > 0);
  assert(sourceChannels > 0);
  assert(0 <= destChannel && destChannel < destChannels);
  assert(0 <= sourceChannel && sourceChannel < sourceChannels);
  uint8_t* di = static_cast<uint8_t*>(dest);
  const uint8_t* si = static_cast<const uint8_t*>(source);
  char** dn = static_cast<char**>(dest);
  const uint8_t* const* sn = static_cast<const uint8_t* const*>(source);

  if(interleaved)
    for(int32_t f = 0; f < frames; f++)
      memcpy(
        &di[(f * destChannels + destChannel) * sampleSize], 
        &si[(f * sourceChannels + sourceChannel) * sampleSize], 
        sampleSize);
  else
    for(int32_t f = 0; f < frames; f++)
      memcpy(
        &dn[destChannel][f * sampleSize], 
        &sn[sourceChannel][f * sampleSize],
        sampleSize);
}

static void ZeroBuffer(
  void* buffer, XtBool interleaved,
  int32_t posFrames, int32_t channels, 
  int32_t frames, int32_t sampleSize) {

  int32_t ss = sampleSize;
  int32_t fs = channels * ss;
  if(frames > 0)
    if(interleaved)
      memset(static_cast<uint8_t*>(buffer) + posFrames * fs, 0, frames * fs);
    else
      for(int32_t i = 0; i < channels; i++)
        memset(static_cast<uint8_t**>(buffer)[i] + posFrames * ss, 0, frames * ss);
}

// ---- ring buffer ----

// ---- aggregate ----

// ---- sync callbacks ---

void XT_CALLBACK XtiOnSlaveBuffer(
  const XtStream* stream, const XtBuffer* buffer, void* user) {

  size_t i;
  int32_t read, written;
  auto ctx = static_cast<XtAggregateContext*>(user);
  int32_t index = ctx->index;
  XtAggregate* aggregate = ctx->stream;
  auto sampleSize = XtiGetSampleSize(aggregate->_params.format.mix.sample);
  XtOnXRun onXRun = aggregate->_params.stream.onXRun;
  XtRingBuffer& inputRing = aggregate->inputRings[index];
  XtRingBuffer& outputRing = aggregate->outputRings[index];
  const XtChannels& channels = aggregate->channels[index];

  aggregate->insideCallbackCount++;
  if(buffer->error != 0) {
    for(i = 0; i < aggregate->streams.size(); i++)
      if(i != static_cast<size_t>(index))
        aggregate->streams[i]->RequestStop();
    aggregate->_params.stream.onBuffer(aggregate, buffer, aggregate->_user);
  } else {

    if(aggregate->running.load() != 1) {
      ZeroBuffer(buffer->output, aggregate->_params.stream.interleaved, 0, channels.outputs, buffer->frames, sampleSize);
    } else {

      if(buffer->input != nullptr) {
        inputRing.Lock();
        written = inputRing.Write(buffer->input, buffer->frames);
        inputRing.Unlock();
        if(written < buffer->frames && onXRun != nullptr)
          onXRun(-1, aggregate->_user);
      }
  
      if(buffer->output != nullptr) {
        outputRing.Lock();
        read = outputRing.Read(buffer->output, buffer->frames);
        outputRing.Unlock();
        if(read < buffer->frames) {
          ZeroBuffer(buffer->output, aggregate->_params.stream.interleaved, read, channels.outputs, buffer->frames - read, sampleSize);
          if(onXRun != nullptr)
            onXRun(-1, aggregate->_user);
        }
      }
    }
  }
  aggregate->insideCallbackCount--;
}

void XT_CALLBACK XtiOnMasterBuffer(
  const XtStream* stream, const XtBuffer* buffer, void* user) {

  size_t i;
  void* appInput;
  void* appOutput;
  void* ringInput;
  void* ringOutput;
  XtRingBuffer* thisInRing;
  XtRingBuffer* thisOutRing;
  const XtStream* thisStream;
  const XtFormat* thisFormat;
  int32_t c, read, written, totalChannels;

  auto ctx = static_cast<XtAggregateContext*>(user);
  int32_t index = ctx->index;
  XtAggregate* aggregate = ctx->stream;
  int32_t sampleSize = XtiGetSampleSize(aggregate->_params.format.mix.sample);
  XtBool interleaved = aggregate->_params.stream.interleaved;
  const XtFormat* format = &aggregate->_params.format;
  XtOnXRun onXRun = aggregate->_params.stream.onXRun;
  XtRingBuffer& inputRing = aggregate->inputRings[index];
  XtRingBuffer& outputRing = aggregate->outputRings[index];
  const XtChannels& channels = aggregate->channels[index];

  for(i = 0; i < aggregate->streams.size(); i++)
    if(i != static_cast<size_t>(aggregate->masterIndex))
      static_cast<XtBlockingStream*>(aggregate->streams[i].get())->ProcessBuffer(false);

  XtiOnSlaveBuffer(stream, buffer, user);
  if(buffer->error != 0) {
    return;
  }

  if(aggregate->running.load() != 1)
    return;

  aggregate->insideCallbackCount++;

  ringInput = interleaved? 
    static_cast<void*>(&(aggregate->_weave.input.interleaved[0])):
    &(aggregate->_weave.input.nonInterleaved[0]);
  ringOutput = interleaved? 
    static_cast<void*>(&(aggregate->_weave.output.interleaved[0])):
    &(aggregate->_weave.output.nonInterleaved[0]);
  appInput = format->channels.inputs == 0?
    nullptr:
    interleaved? 
    static_cast<void*>(&(aggregate->_buffers.input.interleaved[0])):
    &(aggregate->_buffers.input.nonInterleaved[0]);
  appOutput = format->channels.outputs == 0?
    nullptr:
    interleaved? 
    static_cast<void*>(&(aggregate->_buffers.output.interleaved[0])):
    &(aggregate->_buffers.output.nonInterleaved[0]);

  totalChannels = 0;
  for(i = 0; i < aggregate->streams.size(); i++) {
    thisInRing = &aggregate->inputRings[i];
    thisStream = aggregate->streams[i].get();
    thisFormat = &aggregate->streams[i]->_params.format;
    if(thisFormat->channels.inputs > 0) {
      thisInRing->Lock();
      read = thisInRing->Read(ringInput, buffer->frames);
      thisInRing->Unlock();
      if(read < buffer->frames) {
        ZeroBuffer(ringInput, interleaved, read, thisFormat->channels.inputs, buffer->frames - read, sampleSize);
        if(onXRun != nullptr)
          onXRun(-1, aggregate->_user);
      }
      for(c = 0; c < thisFormat->channels.inputs; c++)
        Weave(appInput, ringInput, interleaved, format->channels.inputs, thisFormat->channels.inputs, totalChannels + c, c, buffer->frames, sampleSize);
      totalChannels += thisFormat->channels.inputs;
    }
  }

  XtBuffer appBuffer = *buffer;
  appBuffer.input = appInput;
  appBuffer.output = appOutput;
  aggregate->_params.stream.onBuffer(aggregate, &appBuffer, aggregate->_user);

  totalChannels = 0;
  for(i = 0; i < aggregate->streams.size(); i++) {
    thisOutRing = &aggregate->outputRings[i];
    thisStream = aggregate->streams[i].get();
    thisFormat = &aggregate->streams[i]->_params.format;
    if(thisFormat->channels.outputs > 0) {
      for(c = 0; c < thisFormat->channels.outputs; c++)
        Weave(ringOutput, appOutput, interleaved, thisFormat->channels.outputs, format->channels.outputs, c, totalChannels + c, buffer->frames, sampleSize);
      totalChannels += thisFormat->channels.outputs;
      thisOutRing->Lock();
      written = thisOutRing->Write(ringOutput, buffer->frames);
      thisOutRing->Unlock();
      if(written < buffer->frames && onXRun != nullptr)
        onXRun(-1, aggregate->_user);
    }
  }

  aggregate->insideCallbackCount--;
}