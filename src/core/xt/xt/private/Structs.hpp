#ifndef XT_PRIVATE_STRUCTS_HPP
#define XT_PRIVATE_STRUCTS_HPP

#include <vector>
#include <atomic>
#include <cstdint>

struct XtBuffers
{
  std::vector<uint8_t> interleaved;
  std::vector<void*> nonInterleaved;
  std::vector<std::vector<uint8_t>> channels;
};

struct XtIOBuffers
{
  XtBuffers input;
  XtBuffers output;
};

struct XtAtomicInt
{
  std::atomic_int v;
  XtAtomicInt() = default;
  XtAtomicInt(XtAtomicInt const& i):
  v(i.v.load()) { }

  XtAtomicInt& operator=(XtAtomicInt const& i)
  { v = i.v.load(); return *this; }
};

#endif // XT_PRIVATE_STRUCTS_HPP