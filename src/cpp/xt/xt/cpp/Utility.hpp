#ifndef XT_CPP_UTILITY_HPP
#define XT_CPP_UTILITY_HPP

#include <xt/cpp/Core.hpp>
#include <xt/cpp/XtStream.hpp>
#include <xt/cpp/XtException.hpp>

namespace Xt::Detail {

inline OnError _onError = nullptr;

inline void HandleError(XtError error) 
{ if(error != 0) throw Exception(error); }
inline void XT_CALLBACK ForwardOnError(char const* location, char const* message) 
{ if(_onError) _onError(location, message); }
inline void XT_CALLBACK ForwardOnXRun(int32_t index, void* user) 
{ static_cast<Stream*>(user)->_onXRun(index, static_cast<Stream*>(user)->_user); }

inline void XT_CALLBACK 
ForwardOnBuffer(XtStream const* coreStream, XtBuffer const* coreBuffer, void* user)
{
  Buffer buffer = { 0 };
  auto stream = static_cast<Stream*>(user);
  buffer.time = coreBuffer->time;
  buffer.input = coreBuffer->input; 
  buffer.error = coreBuffer->error;
  buffer.output = coreBuffer->output; 
  buffer.frames = coreBuffer->frames;
  buffer.position = coreBuffer->position;
  buffer.timeValid = coreBuffer->timeValid != XtFalse;
  stream->_onBuffer(*stream, buffer, stream->_user);
}

} // namespace Xt::Detail
#endif // XT_CPP_UTILITY_HPP