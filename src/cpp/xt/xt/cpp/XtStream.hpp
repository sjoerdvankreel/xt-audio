#ifndef XT_CPP_STREAM_HPP
#define XT_CPP_STREAM_HPP

/** @file */

#include <xt/cpp/Core.hpp>
#include <xt/cpp/Forward.hpp>
#include <xt/cpp/Structs.hpp>
#include <xt/cpp/Callbacks.hpp>

#include <cstdint>

namespace Xt {

using namespace Detail;

class Stream final 
{
  XtStream* _s;
  void* const _user;
  StreamParams const _params;

  Stream(StreamParams const& params, void* user):
  _s(nullptr), _params(params), _user(user) { }

public:
  ~Stream();
  void Stop();
  void Start();
  bool IsRunning() const;
  int32_t GetFrames() const;
  Latency GetLatency() const;
  Format const& GetFormat() const;

  friend class Device;
  friend class Service;
  friend void XT_CALLBACK 
  Detail::ForwardOnXRun(XtStream const* coreStream, int32_t index, void* user);
  friend void XT_CALLBACK 
  Detail::ForwardOnRunning(XtStream const* coreStream, XtBool running, void* user);
  friend uint32_t XT_CALLBACK 
  Detail::ForwardOnBuffer(XtStream const* coreStream, XtBuffer const* coreBuffer, void* user);
};

inline void
Stream::Stop() 
{ XtStreamStop(_s); }
inline
Stream::~Stream() 
{ XtStreamDestroy(_s); }
inline bool
Stream::IsRunning() const
{ return XtStreamIsRunning(_s); }
inline void
Stream::Start() 
{ Detail::HandleError(XtStreamStart(_s)); }

inline int32_t
Stream::GetFrames() const 
{
  int32_t frames;
  Detail::HandleError(XtStreamGetFrames(_s, &frames));
  return frames;
}

inline Format const& 
Stream::GetFormat() const
{
  auto coreFormat = XtStreamGetFormat(_s);
  return *reinterpret_cast<Format const*>(coreFormat);
}

inline Latency
Stream::GetLatency() const
{
  Latency latency;
  auto coreLatency = reinterpret_cast<XtLatency*>(&latency);
  Detail::HandleError(XtStreamGetLatency(_s, coreLatency));
  return latency;
}

} // namespace Xt
#endif // XT_CPP_STREAM_HPP