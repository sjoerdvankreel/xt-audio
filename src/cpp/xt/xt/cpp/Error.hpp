#ifndef XT_CPP_ERROR_HPP
#define XT_CPP_ERROR_HPP

#include <xt/cpp/Core.hpp>
#include <xt/api/XtException.hpp>

#include <utility>
#include <iostream>
#include <stdexcept>
#include <exception>
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

inline void 
HandleAssert()
{ 
  char const* error = XtAudioGetLastAssert();
  if(error != nullptr) throw std::logic_error(error); 
}

template <class T> inline T
HandleAssert(T result)
{
  HandleAssert();
  return result;
}

inline void 
HandleError(XtError error) 
{ 
  HandleAssert();
  if(error != 0) throw Exception(error); 
}

template <class F, class... Args> inline void
HandleAssert(F f, Args&&... args)
{
  f(std::forward<Args>(args)...);
  HandleAssert();
}

template <class F, class... Args> inline void
HandleDestroy(F f, Args&&... args)
{
  f(std::forward<Args>(args)...);
  char const* error = XtAudioGetLastAssert(); 
  if(error == nullptr) return;
  std::cerr << XtAudioGetLastAssert() << std::endl;
  std::terminate();
}

} // namespace Xt::Detail
#endif // XT_CPP_ERROR_HPP