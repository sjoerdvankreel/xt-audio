#ifndef XT_CORE_CALLBACKS_HPP
#define XT_CORE_CALLBACKS_HPP

#include <string>
#include <cstdint>

namespace Xt {

typedef void (*OnXRun)(int32_t index, void* user);
typedef void (*OnError)(std::string const& location, std::string const& message);
typedef void (*OnBuffer)(class Stream const& stream, struct Buffer const& buffer, void* user);

} // namespace Xt
#endif // XT_CORE_CALLBACKS_HPP