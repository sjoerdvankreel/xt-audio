#ifndef XT_API_EXCEPTION_HPP
#define XT_API_EXCEPTION_HPP

/** @file */
/** @cond */
#include <stdexcept>
/** @endcond */

namespace Xt {

class Exception final: 
public std::exception 
{
  uint64_t const _error;
public:
  char const* what() const noexcept override;
  Exception(uint64_t error): _error(error) { }
  uint64_t GetError() const { return _error; }
};

inline char const*
Exception::what() const noexcept
{
  Detail::XtErrorInfo coreInfo = Detail::XtAudioGetErrorInfo(_error);
  return Detail::XtPrintErrorInfo(&coreInfo);
}

} // namespace Xt
#endif // XT_API_EXCEPTION_HPP