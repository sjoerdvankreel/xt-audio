#include <xt/private/RingBuffer.hpp>
#include <cstring>

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
    auto count = frames * channels * size;
    std::vector<uint8_t> buffer(count, 0);
    _blocks = std::vector<std::vector<uint8_t>>(1, buffer);
  } else
  {
    auto count = frames * size;
    std::vector<uint8_t> channel(count, 0);
    _blocks = std::vector<std::vector<uint8_t>>(channels, channel);
  }
}

int32_t
XtRingBuffer::Read(void* target, int32_t frames)
{
  int32_t i;
  assert(_locked.v.load());
  assert(0 <= _full && _full <= _frames);

  int32_t frameSize = _channels * _sampleSize;
  int32_t beginFrames = _begin * frameSize;
  int32_t beginSamples = _begin * _sampleSize;
  int32_t result = _full > frames? frames: _full;
  uint8_t* ilTarget = static_cast<uint8_t*>(target);
  uint8_t** niTarget = static_cast<uint8_t**>(target);
  
  if(_end > _begin)
  {
    int32_t resultFrames = result * frameSize;
    int32_t resultSamples = result * _sampleSize;
    if(_interleaved)
      memcpy(ilTarget, &(_blocks[0][beginFrames]), resultFrames);
    else for(i = 0; i < _channels; i++)
      memcpy(niTarget[i], &(_blocks[i][beginSamples]), resultSamples);
  } else 
  {
    int32_t split = result > _frames - _begin? _frames - _begin: result;
    int32_t wrap = result - split;
    int32_t wrapFrames = wrap * frameSize;
    int32_t splitFrames = split * frameSize;
    int32_t wrapSamples = wrap * _sampleSize;
    int32_t splitSamples = split * _sampleSize;
    if(_interleaved)
    {
      memcpy(ilTarget, &(_blocks[0][beginFrames]), splitFrames);
      if(_frames - _begin < result)
        memcpy(ilTarget + splitFrames, &(_blocks[0][0]), wrapFrames);
    } else for(i = 0; i < _channels; i++) 
    {
      memcpy(niTarget[i], &(_blocks[i][beginSamples]), splitSamples);
      if(_frames - _begin < result)
        memcpy(niTarget[i] + splitSamples, &(_blocks[i][0]), wrapSamples);
    }
  }

  _full -= result;
  _begin += result;
  if(_begin >= _frames) _begin -= _frames;
  assert(0 <= _full && _full <= _frames);
  assert(_locked.v.load());
  return result;
}

int32_t
XtRingBuffer::Write(void const* source, int32_t frames)
{
  int32_t i;
  assert(_locked.v.load());
  assert(0 <= _full && _full <= _frames);

  int32_t empty = _frames - _full;
  int32_t result = empty > frames? frames: empty;
  int32_t frameSize = _channels * _sampleSize;
  int32_t endFrames = _end * frameSize;
  int32_t endSamples = _end * _sampleSize;  
  auto ilSource = static_cast<uint8_t const*>(source);
  auto niSource = static_cast<uint8_t const* const*>(source);

  if (_end < _begin)
  {
    int32_t resultFrames = result * frameSize;
    int32_t resultSamples = result * _sampleSize;
    if(_interleaved)
      memcpy(&(_blocks[0][endFrames]), ilSource, resultFrames);
    else for(i = 0; i < _channels; i++)
      memcpy(&(_blocks[i][endSamples]), niSource[i], resultSamples);
  } else 
  {
    int32_t split = result > _frames - _end? _frames - _end: result;
    int32_t wrap = result - split;
    int32_t wrapFrames = wrap * frameSize;
    int32_t splitFrames = split * frameSize;
    int32_t wrapSamples = wrap * _sampleSize;
    int32_t splitSamples = split * _sampleSize;
    if(_interleaved) 
    {
      memcpy(&(_blocks[0][endFrames]), ilSource, splitFrames);
      if(_frames - _end < result)
        memcpy(&(_blocks[0][0]), ilSource + splitFrames, wrapFrames);
    } else for(i = 0; i < _channels; i++) 
    {
      memcpy(&(_blocks[i][endSamples]), niSource[i], splitSamples);
      if(_frames - _end < result)
        memcpy(&(_blocks[i][0]), niSource[i] + splitSamples, wrapSamples);
    }
  }

  _end += result;
  _full += result;
  if (_end >= _frames) _end -= _frames;
  assert(0 <= _full && _full <= _frames);
  assert(_locked.v.load());
  return result;
}