#ifndef XT_PRIVATE_RING_BUFFER_HPP
#define XT_PRIVATE_RING_BUFFER_HPP

#include <xt/private/Shared.hpp>
#include <xt/private/Structs.hpp>
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
  inline void Lock() const;
  inline void Unlock() const;
  inline int32_t Full() const;

  int32_t Read(void* target, int32_t frames);
  int32_t Write(void const* source, int32_t frames);

  XtRingBuffer() = default;
  XtRingBuffer(bool interleaved, int32_t frames, int32_t channels, int32_t size);
};

inline void
XtRingBuffer::Clear() 
{
  assert(_locked.v.load());
  _begin = _end = _full = 0;
  assert(_locked.v.load());
}

inline int32_t
XtRingBuffer::Full() const 
{
  int32_t result;
  assert(_locked.v.load());
  result = _full;
  assert(_locked.v.load());
  return result;
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

#endif // XT_PRIVATE_RING_BUFFER_HPP