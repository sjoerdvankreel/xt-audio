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
end(0), full(0), blocks(), begin(0), locked(0),
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

void XtRingBuffer::Lock() const {
  while(XtiCas(&locked, 1, 0) != 0);
}

void XtRingBuffer::Unlock() const {
  XT_ASSERT(XtiCas(&locked, 0, 1) == 1);
}

void XtRingBuffer::Clear() {
  assert(locked);
  end = 0;
  full = 0;
  begin = 0;
  assert(locked);
}

int32_t XtRingBuffer::Full() const {
  int32_t result;
  assert(locked);
  result = full;
  assert(locked);
  return result;
}

int32_t XtRingBuffer::Read(void* target, int32_t frames) {

  int32_t i;
  int32_t result;
  int32_t wrapSize;
  int32_t splitSize;
  int32_t frameSize = channels * sampleSize;
  char* ilTarget = static_cast<char*>(target);
  char** niTarget = static_cast<char**>(target);

  assert(locked);
  assert(0 <= full && full <= this->frames);
  result = full > frames? frames: full;
  
  if(end > begin) {
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

  full -= result;
  begin += result;
  if(begin >= this->frames)
    begin -= this->frames;
  assert(locked);
  assert(0 <= full && full <= this->frames);
  return result;
}

int32_t XtRingBuffer::Write(const void* source, int32_t frames) {

  int32_t i;
  int32_t empty;
  int32_t result;
  int32_t wrapSize;
  int32_t splitSize;
  int32_t frameSize = channels * sampleSize;
  const char* ilSource = static_cast<const char*>(source);
  const char* const* niSource = static_cast<const char* const*>(source);

  assert(locked);
  assert(0 <= full && full <= this->frames);
  empty = this->frames - full;
  result = empty > frames? frames: empty;

  if (end >= begin) {
    splitSize = result > this->frames - end? this->frames - end: result;
    wrapSize = result - splitSize;
    if(interleaved) {
      memcpy(&(blocks[0][end * frameSize]), ilSource, splitSize * frameSize);
      if(this->frames - end < result)
        memcpy(&(blocks[0][0]), ilSource + splitSize * frameSize, wrapSize * frameSize);
    } else {
      for(i = 0; i < channels; i++) {
        memcpy(&(blocks[i][end * sampleSize]), niSource[i], splitSize * sampleSize);
        if(this->frames - end < result)
          memcpy(&(blocks[i][0]), niSource[i] + splitSize * sampleSize, wrapSize * sampleSize);
      }
    }
  } else {
    if(interleaved)
      memcpy(&(blocks[0][end * frameSize]), ilSource, result * frameSize);
    else
      for(i = 0; i < channels; i++)
        memcpy(&(blocks[i][end * sampleSize]), niSource[i], result * sampleSize);
  }

  end += result;
  full += result;
  if (end >= this->frames)
    end -= this->frames;
  assert(locked);
  assert(0 <= full && full <= this->frames);
  return result;
}

// ---- aggregate ----

XtAggregate::~XtAggregate() {
  Stop();
}

XtSystem XtAggregate::GetSystem() const {
  return system;
}

XtFault XtAggregate::GetFrames(int32_t* frames) const {
  *frames = this->frames;
  return 0;
}

XtFault XtAggregate::Stop() {
  XtError error;
  XtError result = 0;
  XtiCas(&running, 0, 1);
  while(XtiCas(&insideCallbackCount, 0, 0) != 0);
  for(size_t i = 0; i < streams.size(); i++)
    if((error = XtStreamStop(streams[i].get())) != 0)
      result = error;
  return XtErrorGetFault(result);
}

XtFault XtAggregate::Start() {
  XtError error;
  for(size_t i = 0; i < streams.size(); i++) {
    inputRings[i].Lock();
    inputRings[i].Clear();
    inputRings[i].Unlock();
    outputRings[i].Lock();
    outputRings[i].Clear();
    outputRings[i].Unlock();
  }
  for(size_t i = 0; i < streams.size(); i++)
    if((error = XtStreamStart(streams[i].get()) != 0)) {
      Stop();
      return XtErrorGetFault(error);
    }
  XtiCas(&running, 1, 0);
  return 0;
}

XtFault XtAggregate::GetLatency(XtLatency* latency) const {
  XtFault fault;
  XtLatency local = { 0 };
  for(size_t i = 0; i < streams.size(); i++) {
    if((fault = streams[i]->GetLatency(&local)) != 0)
      return fault;
    if(local.input == 0.0 && local.output == 0.0)
      return 0;
    if(local.input > 0.0) {
      inputRings[i].Lock();
      local.input += inputRings[i].Full() * 1000.0 / format.mix.rate;
      inputRings[i].Unlock();
      latency->input = local.input > latency->input? local.input: latency->input;
    }      
    if(local.output > 0.0) {
      outputRings[i].Lock();
      local.output += outputRings[i].Full() * 1000.0 / format.mix.rate;
      outputRings[i].Unlock();
      latency->output = local.output > latency->output? local.output: latency->output;
    }      
  }
  return 0;
}

// ---- sync callbacks ---

void XT_CALLBACK XtiSlaveCallback(
  const XtStream* stream, const void* input, void* output, int32_t frames,
  double time, uint64_t position, XtBool timeValid, XtError error, void* user) {

  size_t i;
  int32_t read, written;
  auto ctx = static_cast<XtAggregateContext*>(user);
  int32_t index = ctx->index;
  XtAggregate* aggregate = ctx->stream;
  XtXRunCallback xRunCallback = aggregate->xRunCallback;
  XtRingBuffer& inputRing = aggregate->inputRings[index];
  XtRingBuffer& outputRing = aggregate->outputRings[index];
  const XtChannels& channels = aggregate->channels[index];

  XtiLockIncr(&aggregate->insideCallbackCount);

  if(error != 0) {
    for(i = 0; i < aggregate->streams.size(); i++)
      if(i != static_cast<size_t>(index))
        aggregate->streams[i]->RequestStop();
    aggregate->streamCallback(aggregate, nullptr, nullptr, 0, 0.0, 0, XtFalse, error, aggregate->user);
  } else {

    if(XtiCas(&aggregate->running, 1, 1) != 1) {
      ZeroBuffer(output, aggregate->interleaved, 0, channels.outputs, frames, aggregate->sampleSize);
    } else {

      if(input != nullptr) {
        inputRing.Lock();
        written = inputRing.Write(input, frames);
        inputRing.Unlock();
        if(written < frames && xRunCallback != nullptr)
          xRunCallback(-1, XtFalse, XtTrue, frames - written, aggregate->user);
      }
  
      if(output != nullptr) {
        outputRing.Lock();
        read = outputRing.Read(output, frames);
        outputRing.Unlock();
        if(read < frames) {
          ZeroBuffer(output, aggregate->interleaved, read, channels.outputs, frames - read, aggregate->sampleSize);
          if(xRunCallback != nullptr)
            xRunCallback(-1, XtTrue, XtFalse, frames - read, aggregate->user);
        }
      }
    }
  }

  XtiLockDecr(&aggregate->insideCallbackCount);
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

  XtiSlaveCallback(stream, input, output, frames, time, position, timeValid, error, user);
  if(error != 0) {
    return;
  }

  if(XtiCas(&aggregate->running, 1, 1) != 1)
    return;

  XtiLockIncr(&aggregate->insideCallbackCount);

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
      read = thisInRing->Read(ringInput, frames);
      thisInRing->Unlock();
      if(read < frames) {
        ZeroBuffer(ringInput, interleaved, read, thisFormat->inputs, frames - read, sampleSize);
        if(xRunCallback != nullptr)
          xRunCallback(-1, XtFalse, XtFalse, frames - read, aggregate->user);
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
        Weave(ringOutput, appOutput, interleaved, thisFormat->outputs, format->outputs, c, totalChannels + c, frames, sampleSize);
      totalChannels += thisFormat->outputs;
      thisOutRing->Lock();
      written = thisOutRing->Write(ringOutput, frames);
      thisOutRing->Unlock();
      if(written < frames && xRunCallback != nullptr)
        xRunCallback(-1, XtTrue, XtTrue, frames - written, aggregate->user);
    }
  }

  XtiLockDecr(&aggregate->insideCallbackCount);
}
