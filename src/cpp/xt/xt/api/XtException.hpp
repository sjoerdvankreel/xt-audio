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
  Exception(uint64_t error): _error(error) {}
  uint64_t GetError() const { return _error; }
};

} // namespace Xt
#endif // XT_API_EXCEPTION_HPP