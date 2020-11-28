#include <xt/Private.hpp>
#include <xt/private/Platform.hpp>
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

XtRingBuffer::XtRingBuffer(): 
end(), full(), begin(), frames(), channels(), 
interleaved(), sampleSize(), locked(), blocks() {}

XtRingBuffer::XtRingBuffer(
  bool interleaved, int32_t frames, 
  int32_t channels, int32_t sampleSize):
end(0), full(0), blocks(), begin(0), locked(0),
frames(frames), channels(channels),
sampleSize(sampleSize), interleaved(interleaved) {

  if(!interleaved) {
    std::vector<uint8_t> channel = std::vector<uint8_t>(frames * sampleSize, 0);
    blocks = std::vector<std::vector<uint8_t>>(channels, channel);
  } else {
    std::vector<uint8_t> buffer = std::vector<uint8_t>(frames * channels * sampleSize, 0);
    blocks = std::vector<std::vector<uint8_t>>(1, buffer);
  }
}

void XtRingBuffer::Lock() const {
  while(XtPlatform::Cas(&locked, 1, 0) != 0);
}

void XtRingBuffer::Unlock() const {
  XT_ASSERT(XtPlatform::Cas(&locked, 0, 1) == 1);
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
  uint8_t* ilTarget = static_cast<uint8_t*>(target);
  uint8_t** niTarget = static_cast<uint8_t**>(target);

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
  const uint8_t* ilSource = static_cast<const uint8_t*>(source);
  const uint8_t* const* niSource = static_cast<const uint8_t* const*>(source);

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
  XtPlatform::Cas(&running, 0, 1);
  while(XtPlatform::Cas(&insideCallbackCount, 0, 0) != 0);
  if(masterIndex == -1 || static_cast<size_t>(masterIndex) >= streams.size())
    return 0;
  if((error = XtStreamStop(streams[masterIndex].get())) != 0)
    result = error;
  for(size_t i = 0; i < streams.size(); i++)
    if(i != static_cast<size_t>(masterIndex))
      if((error = XtStreamStop(streams[i].get())) != 0)
        result = error;
  return XtiGetErrorFault(result);
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
    if(i != static_cast<size_t>(masterIndex))
      if((error = XtStreamStart(streams[i].get()) != 0)) {
        Stop();
        return XtiGetErrorFault(error);
      }
  if((error = XtStreamStart(streams[masterIndex].get()) != 0)) {
    Stop();
    return XtiGetErrorFault(error);
  }
  XtPlatform::Cas(&running, 1, 0);
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
      local.input += inputRings[i].Full() * 1000.0 / _params.format.mix.rate;
      inputRings[i].Unlock();
      latency->input = local.input > latency->input? local.input: latency->input;
    }      
    if(local.output > 0.0) {
      outputRings[i].Lock();
      local.output += outputRings[i].Full() * 1000.0 / _params.format.mix.rate;
      outputRings[i].Unlock();
      latency->output = local.output > latency->output? local.output: latency->output;
    }      
  }
  return 0;
}

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

  XtPlatform::LockIncr(&aggregate->insideCallbackCount);

  if(buffer->error != 0) {
    for(i = 0; i < aggregate->streams.size(); i++)
      if(i != static_cast<size_t>(index))
        aggregate->streams[i]->RequestStop();
    aggregate->_params.stream.onBuffer(aggregate, buffer, aggregate->_internal.user);
  } else {

    if(XtPlatform::Cas(&aggregate->running, 1, 1) != 1) {
      ZeroBuffer(buffer->output, aggregate->_params.stream.interleaved, 0, channels.outputs, buffer->frames, sampleSize);
    } else {

      if(buffer->input != nullptr) {
        inputRing.Lock();
        written = inputRing.Write(buffer->input, buffer->frames);
        inputRing.Unlock();
        if(written < buffer->frames && onXRun != nullptr)
          onXRun(-1, aggregate->_internal.user);
      }
  
      if(buffer->output != nullptr) {
        outputRing.Lock();
        read = outputRing.Read(buffer->output, buffer->frames);
        outputRing.Unlock();
        if(read < buffer->frames) {
          ZeroBuffer(buffer->output, aggregate->_params.stream.interleaved, read, channels.outputs, buffer->frames - read, sampleSize);
          if(onXRun != nullptr)
            onXRun(-1, aggregate->_internal.user);
        }
      }
    }
  }

  XtPlatform::LockDecr(&aggregate->insideCallbackCount);
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

  if(aggregate->streams[aggregate->masterIndex]->IsBlocking())
    for(i = 0; i < aggregate->streams.size(); i++)
      if(i != static_cast<size_t>(aggregate->masterIndex))
        static_cast<XtBlockingStream*>(aggregate->streams[i].get())->ProcessBuffer(false);

  XtiOnSlaveBuffer(stream, buffer, user);
  if(buffer->error != 0) {
    return;
  }

  if(XtPlatform::Cas(&aggregate->running, 1, 1) != 1)
    return;

  XtPlatform::LockIncr(&aggregate->insideCallbackCount);

  ringInput = interleaved? 
    static_cast<void*>(&(aggregate->_weave.input.interleaved[0])):
    &(aggregate->_weave.input.nonInterleaved[0]);
  ringOutput = interleaved? 
    static_cast<void*>(&(aggregate->_weave.output.interleaved[0])):
    &(aggregate->_weave.output.nonInterleaved[0]);
  appInput = format->channels.inputs == 0?
    nullptr:
    interleaved? 
    static_cast<void*>(&(aggregate->_internal.buffers.input.interleaved[0])):
    &(aggregate->_internal.buffers.input.nonInterleaved[0]);
  appOutput = format->channels.outputs == 0?
    nullptr:
    interleaved? 
    static_cast<void*>(&(aggregate->_internal.buffers.output.interleaved[0])):
    &(aggregate->_internal.buffers.output.nonInterleaved[0]);

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
          onXRun(-1, aggregate->_internal.user);
      }
      for(c = 0; c < thisFormat->channels.inputs; c++)
        Weave(appInput, ringInput, interleaved, format->channels.inputs, thisFormat->channels.inputs, totalChannels + c, c, buffer->frames, sampleSize);
      totalChannels += thisFormat->channels.inputs;
    }
  }

  XtBuffer appBuffer = *buffer;
  appBuffer.input = appInput;
  appBuffer.output = appOutput;
  aggregate->_params.stream.onBuffer(aggregate, &appBuffer, aggregate->_internal.user);

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
        onXRun(-1, aggregate->_internal.user);
    }
  }

  XtPlatform::LockDecr(&aggregate->insideCallbackCount);
}