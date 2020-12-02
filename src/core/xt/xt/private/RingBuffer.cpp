#include <xt/private/RingBuffer.hpp>

XtRingBuffer::
XtRingBuffer(
  bool interleaved, int32_t frames,
  int32_t channels, int32_t size):
_end(0), _full(0), _begin(0), _frames(frames),
_channels(channels), _interleaved(interleaved),
_sampleSize(size), _locked(), _blocks()
{
  if(interleaved)
  {
    std::vector<uint8_t> buffer(frames * channels * size, 0);
    _blocks = std::vector<std::vector<uint8_t>>(1, buffer);
  } else
  {
    std::vector<uint8_t> channel(frames * size, 0);
    _blocks = std::vector<std::vector<uint8_t>>(channels, channel);
  }
}

int32_t
XtRingBuffer::Read(void* target, int32_t frames)
{
  int32_t i;
  int32_t result;
  int32_t wrapSize;
  int32_t splitSize;
  int32_t frameSize = channels * sampleSize;
  uint8_t* ilTarget = static_cast<uint8_t*>(target);
  uint8_t** niTarget = static_cast<uint8_t**>(target);

  assert(locked.v.load());
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
  assert(locked.v.load());
  assert(0 <= full && full <= this->frames);
  return result;
}

int32_t
XtRingBuffer::Write(const void* source, int32_t frames)
{

  int32_t i;
  int32_t empty;
  int32_t result;
  int32_t wrapSize;
  int32_t splitSize;
  int32_t frameSize = channels * sampleSize;
  const uint8_t* ilSource = static_cast<const uint8_t*>(source);
  const uint8_t* const* niSource = static_cast<const uint8_t* const*>(source);

  assert(locked.v.load());
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
  assert(locked.v.load());
  assert(0 <= full && full <= this->frames);
  return result;
}