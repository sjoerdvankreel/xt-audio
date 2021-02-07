#ifndef XT_CPP_UTILITY_HPP
#define XT_CPP_UTILITY_HPP

#include <xt/cpp/Core.hpp>
#include <xt/api/Structs.hpp>
#include <xt/api/XtStream.hpp>
#include <xt/api/XtException.hpp>

#include <utility>
#include <stdexcept>
#include <type_traits>

namespace Xt::Detail {

struct Initializer
{ Initializer(); };

inline OnError
_onError = nullptr;
inline Initializer
_initializer;

inline void XT_CALLBACK 
ForwardOnError(char const* message) 
{ _onError(message); }
inline Initializer::
Initializer() { XtAudioSetAssertTerminates(XtFalse); }
inline void HandleAssert()
{ if(XtAudioGetLastAssert() != nullptr) throw std::logic_error(XtAudioGetLastAssert()); }

inline void 
HandleError(XtError error) 
{ 
  HandleAssert();
  if(error != 0) throw Exception(error); 
}

template <class F, class... Args> inline void
HandleVoidError(F f, Args&&... args)
{
  f(std::forward<Args>(args)...);
  HandleAssert();
}

template <class F, class... Args> inline decltype(auto)
HandleError(F f, Args&&... args)
{
  auto result = f(std::forward<Args>(args)...);
  HandleAssert();
  return result;
}

inline void XT_CALLBACK 
ForwardOnXRun(XtStream const* coreStream, int32_t index, void* user) 
{
  auto stream = static_cast<Stream*>(user);
  stream->_params.onXRun(*stream, index, stream->_user); 
}

inline uint32_t XT_CALLBACK 
ForwardOnBuffer(XtStream const* coreStream, XtBuffer const* coreBuffer, void* user)
{
  Buffer buffer = { 0 };
  auto stream = static_cast<Stream*>(user);
  buffer.time = coreBuffer->time;
  buffer.input = coreBuffer->input; 
  buffer.output = coreBuffer->output; 
  buffer.frames = coreBuffer->frames;
  buffer.position = coreBuffer->position;
  buffer.timeValid = coreBuffer->timeValid != XtFalse;
  return stream->_params.onBuffer(*stream, buffer, stream->_user);
}

inline void XT_CALLBACK 
ForwardOnRunning(XtStream const* coreStream, XtBool running, uint64_t error, void* user)
{  
  auto stream = static_cast<Stream*>(user);
  stream->_params.onRunning(*stream, running != 0, error, stream->_user);
}

} // namespace Xt::Detail
#endif // XT_CPP_UTILITY_HPP