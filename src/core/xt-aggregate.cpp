#include "xt-private.hpp"
#include <cassert>

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
  char* di = static_cast<char*>(dest);
  const char* si = static_cast<const char*>(source);
  char** dn = static_cast<char**>(dest);
  const char* const* sn = static_cast<const char* const*>(source);

  if(interleaved)
    for(int32_t f = 0; f < frames; f++)
      memcpy(
        &di[(f * destChannels + destChannel) * sampleSize], 
        &si[(f * sourceChannels + sourceChannel)], 
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
  if(interleaved)
    memset(static_cast<char*>(buffer) + posFrames * fs, 0, frames * fs);
  else
    for(int32_t i = 0; i < channels; i++)
      memset(static_cast<char**>(buffer)[i] + posFrames * ss, 0, frames * ss);
}

// ---- ring buffer ----

XtRingBuffer::XtRingBuffer(
  bool interleaved, int32_t frames, 
  int32_t channels, int32_t sampleSize):
end(0), blocks(), begin(0), locked(0),
frames(frames), channels(channels),
sampleSize(sampleSize), interleaved(interleaved) {

  if(!interleaved) {
    std::vector<char> channel = std::vector<char>(frames * sampleSize, '\0');
    blocks = std::vector<std::vector<char>>(channels, channel);
  } else {
    std::vector<char> buffer = std::vector<char>(frames * channels * sampleSize, '\0');
    blocks = std::vector<std::vector<char>>(1, buffer);
  }
}

void XtRingBuffer::Clear() {
 XT_ASSERT(locked);
 begin = 0;
 end = 0;
 XT_ASSERT(locked);
}

void XtRingBuffer::Lock() {
  while(XtiCas(&locked, 1, 0) != 0);
}

void XtRingBuffer::Unlock() {
  XT_ASSERT(XtiCas(&locked, 0, 1) == 1);
}

int32_t XtRingBuffer::Read(void* target, int32_t frames) {

  int32_t i;
  int32_t full;
  int32_t result;
  int32_t wrapSize;
  int32_t splitSize;
  int32_t frameSize = channels * sampleSize;
  char* ilTarget = static_cast<char*>(target);
  char** niTarget = static_cast<char**>(target);

  XT_ASSERT(locked);  
  full = end - begin;
  if(full < 0)
    full += this->frames;
  result = full > frames? frames: full;
  
  if(target != nullptr)
    if(end >= begin) {
      if(interleaved)
        memcpy(ilTarget, &(blocks[0][begin * frameSize]), result * frameSize);
      else
        for(i = 0; i < channels; i++)
          memcpy(niTarget[i], &(blocks[i][begin * sampleSize]), result * sampleSize);
    } else {
      splitSize = result > this->frames - begin? this->frames - begin: result;
      wrapSize = result - splitSize;
      if(interleaved) {
        memcpy(ilTarget, &(blocks[0][begin * frameSize]), splitSize * frameSize);
        if(this->frames - begin < result)
          memcpy(ilTarget + splitSize * frameSize, &(blocks[0][0]), wrapSize * frameSize);
      } else {
        for(i = 0; i < channels; i++) {
          memcpy(niTarget[i], &(blocks[i][begin * sampleSize]), splitSize * sampleSize);
          if(this->frames - begin < result)
            memcpy(niTarget[i] + splitSize * sampleSize, &(blocks[i][0]), wrapSize * sampleSize);
        }
      }
    }

  begin += result;
  if(begin >= this->frames)
    begin -= this->frames;
  XT_ASSERT(locked);
  return result;
}

int32_t XtRingBuffer::Write(const void* source, int32_t frames) {

  int32_t i;
  int32_t full;
  int32_t empty;
  int32_t result;
  int32_t wrapSize;
  int32_t splitSize;
  int32_t frameSize = channels * sampleSize;
  const char* ilSource = static_cast<const char*>(source);
  const char* const* niSource = static_cast<const char* const*>(source);

  XT_ASSERT(locked);
  full = end - begin;
  if(full < 0)
    full += this->frames;
  empty = this->frames - full;
  result = empty > frames? frames: empty;

  if (end >= begin) {
    splitSize = result > this->frames - end? this->frames - end: result;
    wrapSize = result - splitSize;
    if(interleaved) {
      if(source != nullptr)
        memcpy(&(blocks[0][end * frameSize]), ilSource, splitSize * frameSize);
      else
        memset(&(blocks[0][end * frameSize]), 0, splitSize * frameSize);
      if(this->frames - end < result)
        if(source != nullptr)
          memcpy(&(blocks[0][0]), ilSource + splitSize * frameSize, wrapSize * frameSize);
        else
          memset(&(blocks[0][0]), 0, wrapSize * frameSize);
    } else {
      for(i = 0; i < channels; i++) {
        if(source != nullptr)
          memcpy(&(blocks[i][end * sampleSize]), niSource[i], splitSize * sampleSize);
        else
          memset(&(blocks[i][end * sampleSize]), 0, splitSize * sampleSize);
        if(this->frames - end < result)
          if(source != nullptr)
            memcpy(&(blocks[i][0]), niSource[i] + splitSize * sampleSize, wrapSize * sampleSize);
          else
            memset(&(blocks[i][0]), 0, wrapSize * sampleSize);
      }
    }
  } else {
    if(interleaved) {
      if(source != nullptr)
        memcpy(&(blocks[0][end * frameSize]), ilSource, result * frameSize);
      else
        memset(&(blocks[0][end * frameSize]), 0, result * frameSize);
    } else {
      for(i = 0; i < channels; i++)
        if(source != nullptr)
          memcpy(&(blocks[i][end * sampleSize]), niSource[i], result * sampleSize);
        else
          memset(&(blocks[i][end * sampleSize]), 0, result * sampleSize);
    }
  }

  end += result;
  if (end >= this->frames)
    end -= this->frames;
  XT_ASSERT(locked);
  return result;
}

// ---- aggregate ----

XtSystem XtAggregate::GetSystem() const {
  return system;
}

XtFault XtAggregate::Stop() {

  // TODO: wait untill the callback has returned.

  XtFault fault;
  XtFault result = 0;
  for(size_t i = 0; i < streams.size(); i++)
    if((fault = streams[i]->Stop()) != 0)
      result = fault;
  return result;
}

XtFault XtAggregate::Start() {
  XtFault fault;
  for(size_t i = 0; i < streams.size(); i++) {
    inputRings[i].Lock();
    inputRings[i].Clear();
    inputRings[i].Unlock();
    outputRings[i].Lock();
    outputRings[i].Clear();
    outputRings[i].Unlock();
    if((fault = streams[i]->Start()) != 0) {
      Stop();
      return fault;
    }
  }
  return 0;
}

XtFault XtAggregate::GetFrames(int32_t* frames) const {
  *frames = this->frames;
  return 0;
}

XtFault XtAggregate::GetLatency(XtLatency* latency) const {
  XtFault fault;
  if((fault = streams[masterIndex]->GetLatency(latency)) != 0)
    return fault;
  if(latency->input != 0.0)
    latency->input += frames * 1000.0 / format.mix.rate;
  if(latency->output != 0.0)
    latency->output += frames * 1000.0 / format.mix.rate;
  return 0;
}

// ---- sync callbacks ---

void XT_CALLBACK XtiSlaveCallback(
  const XtStream* stream, const void* input, void* output, int32_t frames,
  double time, uint64_t position, XtBool timeValid, XtError error, void* user) {

  int32_t read, written;
  auto ctx = static_cast<XtAggregateContext*>(user);
  int32_t index = ctx->index;
  XtAggregate* aggregate = ctx->stream;
  XtXRunCallback xRunCallback = aggregate->xRunCallback;
  XtRingBuffer& inputRing = aggregate->inputRings[index];
  XtRingBuffer& outputRing = aggregate->outputRings[index];
  const XtChannels& channels = aggregate->channels[index];

  // TODO stop aggregate stream on error

  if(input != nullptr) {
    inputRing.Lock();
    if((written = inputRing.Write(input, frames)) < frames)
      inputRing.Read(nullptr, aggregate->frames / 2);
    inputRing.Unlock();
    if(written < frames && xRunCallback != nullptr)
      xRunCallback(stream, XtFalse, XtTrue, frames - written);
  }
  
  if(output != nullptr) {
    outputRing.Lock();
    if((read = outputRing.Read(output, frames)) < frames)
      outputRing.Write(nullptr, aggregate->frames / 2);
    outputRing.Unlock();
    if(read < frames) {
      ZeroBuffer(output, aggregate->interleaved, read, channels.outputs, frames - read, aggregate->sampleSize);
      if(xRunCallback != nullptr)
        xRunCallback(stream, XtTrue, XtFalse, frames - read);
    }
  }
}

void XT_CALLBACK XtiMasterCallback(
  const XtStream* stream, const void* input, void* output, int32_t frames,
  double time, uint64_t position, XtBool timeValid, XtError error, void* user) {

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
  int32_t sampleSize = aggregate->sampleSize;
  XtBool interleaved = aggregate->interleaved;
  const XtFormat* format = &aggregate->format;
  XtXRunCallback xRunCallback = aggregate->xRunCallback;
  XtRingBuffer& inputRing = aggregate->inputRings[index];
  XtRingBuffer& outputRing = aggregate->outputRings[index];
  const XtChannels& channels = aggregate->channels[index];

  // TODO stop aggregate stream on error

  XtiSlaveCallback(stream, input, output, frames, time, position, timeValid, error, user);

  ringInput = interleaved? 
    static_cast<void*>(&(aggregate->weave.inputInterleaved[0])):
    &(aggregate->weave.inputNonInterleaved[0]);
  ringOutput = interleaved? 
    static_cast<void*>(&(aggregate->weave.outputInterleaved[0])):
    &(aggregate->weave.outputNonInterleaved[0]);
  appInput = format->inputs == 0?
    nullptr:
    interleaved? 
    static_cast<void*>(&(aggregate->intermediate.inputInterleaved[0])):
    &(aggregate->intermediate.inputNonInterleaved[0]);
  appOutput = format->outputs == 0?
    nullptr:
    interleaved? 
    static_cast<void*>(&(aggregate->intermediate.outputInterleaved[0])):
    &(aggregate->intermediate.outputNonInterleaved[0]);

  totalChannels = 0;
  for(i = 0; i < aggregate->streams.size(); i++) {
    thisInRing = &aggregate->inputRings[i];
    thisStream = aggregate->streams[i].get();
    thisFormat = &aggregate->streams[i]->format;
    if(thisFormat->inputs > 0) {
      thisInRing->Lock();
      if((read = thisInRing->Read(ringInput, frames)) < frames)
        thisInRing->Write(nullptr, aggregate->frames / 2);
      thisInRing->Unlock();
      if(read < frames) {
        ZeroBuffer(ringInput, interleaved, read, thisFormat->inputs, frames - read, sampleSize);
        if(xRunCallback != nullptr)
          xRunCallback(aggregate, XtFalse, XtFalse, frames - read);
      }
      for(c = 0; c < thisFormat->inputs; c++)
        Weave(appInput, ringInput, interleaved, format->inputs, thisFormat->inputs, totalChannels + c, c, frames, sampleSize);
      totalChannels += thisFormat->inputs;
    }
  }

  aggregate->streamCallback(aggregate, appInput, appOutput, frames, time, position, timeValid, error, aggregate->user);

  totalChannels = 0;
  for(i = 0; i < aggregate->streams.size(); i++) {
    thisOutRing = &aggregate->outputRings[i];
    thisStream = aggregate->streams[i].get();
    thisFormat = &aggregate->streams[i]->format;
    if(thisFormat->outputs > 0) {
      for(c = 0; c < thisFormat->outputs; c++)
        Weave(ringOutput, appOutput, interleaved, thisFormat->outputs, format->outputs, totalChannels + c, c, frames, sampleSize);
      totalChannels += thisFormat->inputs;
      thisOutRing->Lock();
      if((written = thisOutRing->Write(ringOutput, frames)) < frames)
        thisOutRing->Read(nullptr, aggregate->frames / 2);
      thisOutRing->Unlock();
      if(written < frames && xRunCallback != nullptr)
        xRunCallback(aggregate, XtTrue, XtTrue, frames - written);
    }
  }
}
