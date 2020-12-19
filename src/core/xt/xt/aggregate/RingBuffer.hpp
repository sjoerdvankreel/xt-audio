#ifndef XT_AGGREGATE_RING_BUFFER_HPP
#define XT_AGGREGATE_RING_BUFFER_HPP

#include <xt/shared/Shared.hpp>
#include <xt/shared/Structs.hpp>

#include <cstdint>
#include <cassert>

struct XtRingBuffer 
{
  int32_t _end;
  int32_t _full;
  int32_t _begin;
  int32_t _frames;
  int32_t _channels;
  bool _interleaved;
  int32_t _sampleSize;
  mutable XtAtomicInt _locked;
  std::vector<std::vector<uint8_t>> _blocks;

  inline void Clear();
  inline int32_t Full() const;
  int32_t Read(void* target, int32_t frames);
  int32_t Write(void const* source, int32_t frames);

  XtRingBuffer() = default;
  XtRingBuffer(bool interleaved, int32_t frames, int32_t channels, int32_t size);

private:
  inline void Lock() const;
  inline void Unlock() const;
};

struct XtIORingBuffers
{
  XtRingBuffer input;
  XtRingBuffer output;
};

inline int32_t
XtRingBuffer::Full() const 
{
  int32_t result;
  Lock();
  result = _full;
  Unlock();
  return result;
}

inline void
XtRingBuffer::Clear() 
{
  Lock();
  _begin = _end = _full = 0;
  Unlock();
}

inline void
XtRingBuffer::Unlock() const 
{ 
  bool ok = XtiCompareExchange(_locked.v, 1, 0); 
  assert(ok);
}

inline void
XtRingBuffer::Lock() const 
{ while(!XtiCompareExchange(_locked.v, 0, 1)); }

#endif // XT_AGGREGATE_RING_BUFFER_HPP