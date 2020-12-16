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

struct XtAggregateContext
{
  int32_t index;
  struct XtAggregateStream* stream;
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

template <class Rollback>
struct XtGuard
{
  bool enabled;
  bool committed;
  Rollback rollback;

  void Enable() { enabled = true; }
  void Commit() { committed = true; }

  ~XtGuard() { if(enabled && !committed) rollback(); }
  XtGuard(Rollback rb): enabled(false), committed(false), rollback(rb) { }
  XtGuard(XtGuard&& g): enabled(g.enabled), committed(g.committed), rollback(g.rollback) { g.enabled = false; }
};

#endif // XT_PRIVATE_STRUCTS_HPP