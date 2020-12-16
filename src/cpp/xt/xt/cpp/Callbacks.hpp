#ifndef XT_CPP_CALLBACKS_HPP
#define XT_CPP_CALLBACKS_HPP

/** @file */

#include <string>
#include <cstdint>

namespace Xt {

typedef void (*
OnXRun)(int32_t index, void* user);
typedef void (*
OnRunning)(class Stream const& stream, bool running, void* user);
typedef void (*
OnError)(struct Location const& location, std::string const& message);
typedef uint32_t (*
OnBuffer)(class Stream const& stream, struct Buffer const& buffer, void* user);

} // namespace Xt
#endif // XT_CPP_CALLBACKS_HPP