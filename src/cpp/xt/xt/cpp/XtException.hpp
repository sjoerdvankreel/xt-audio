#ifndef XT_CPP_EXCEPTION_HPP
#define XT_CPP_EXCEPTION_HPP

/** @file */

#include <stdexcept>

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
#endif // XT_CPP_EXCEPTION_HPP