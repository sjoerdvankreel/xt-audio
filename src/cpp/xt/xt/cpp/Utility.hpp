#ifndef XT_CPP_UTILITY_HPP
#define XT_CPP_UTILITY_HPP

#include <xt/cpp/Core.hpp>
#include <xt/api/Structs.hpp>
#include <xt/api/XtStream.hpp>
#include <xt/api/XtException.hpp>

namespace Xt::Detail {

inline OnError _onError = nullptr;

inline void 
HandleError(XtError error) 
{ if(error != 0) throw Exception(error); }

inline void XT_CALLBACK 
ForwardOnError(char const* message) 
{ _onError(message); }

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