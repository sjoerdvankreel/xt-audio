#ifndef XT_PRIVATE_STRUCTS_HPP
#define XT_PRIVATE_STRUCTS_HPP

#include <xt/api/public/Structs.h>
#include <vector>
#include <atomic>
#include <cstdint>

struct XtBlockingParams 
{
  XtFormat format;
  double bufferSize;
  XtBool interleaved;
};

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

template <class Rollback>
struct XtGuard
{
  bool committed;
  Rollback rollback;

  void Commit() { committed = true; }
  ~XtGuard() { if(!committed) rollback(); }
  XtGuard(Rollback rb): committed(false), rollback(rb) { }
  XtGuard(XtGuard&& g): committed(g.committed), rollback(g.rollback) { g.committed = true; }
};

#endif // XT_PRIVATE_STRUCTS_HPP