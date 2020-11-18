#ifndef XT_STREAM_HPP
#define XT_STREAM_HPP

/** @file */

#include <xt/cpp/Core.hpp>
#include <xt/cpp/Forward.hpp>
#include <xt/cpp/CoreStructs.hpp>
#include <xt/cpp/CoreCallbacks.hpp>

#include <cstdint>

namespace Xt {

using namespace Detail;

class Stream final 
{
  XtStream* _s;
  void* const _user;
  OnXRun const _onXRun;
  OnBuffer const _onBuffer;

  Stream(OnBuffer onBuffer, OnXRun onXRun, void* user):
  _s(nullptr), _user(user), _onXRun(onXRun), _onBuffer(onBuffer) { }

public:
  ~Stream();
  void Stop();
  void Start();
  int32_t GetFrames() const;
  Latency GetLatency() const;
  Format const& GetFormat() const;

  friend class Device;
  friend class Service;
  friend void XT_CALLBACK 
  Detail::ForwardOnXRun(int32_t index, void* user);
  friend void XT_CALLBACK 
  Detail::ForwardOnBuffer(XtStream const* coreStream, XtBuffer const* coreBuffer, void* user);
};

inline
Stream::~Stream() 
{ XtStreamDestroy(_s); }

inline void
Stream::Stop() 
{ Detail::HandleError(XtStreamStop(_s)); }

inline void
Stream::Start() 
{ Detail::HandleError(XtStreamStart(_s)); }

inline int32_t Stream::GetFrames() const 
{
  int32_t frames;
  Detail::HandleError(XtStreamGetFrames(_s, &frames));
  return frames;
}

inline Format const& Stream::GetFormat() const
{
  auto coreFormat = XtStreamGetFormat(_s);
  return *reinterpret_cast<Format const*>(coreFormat);
}

inline Latency Stream::GetLatency() const
{
  Latency latency;
  auto coreLatency = reinterpret_cast<XtLatency*>(&latency);
  Detail::HandleError(XtStreamGetLatency(_s, coreLatency));
  return latency;
}

} // namespace Xt
#endif // XT_STREAM_HPP