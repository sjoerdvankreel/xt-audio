#ifndef XT_CPP_FORWARD_HPP
#define XT_CPP_FORWARD_HPP

#include <xt/cpp/Core.hpp>

namespace Xt::Detail {

inline void 
HandleError(XtError error);
inline void XT_CALLBACK 
ForwardOnError(XtLocation const* location, char const* message);
inline void XT_CALLBACK 
ForwardOnXRun(XtStream const* coreStream, int32_t index, void* user);
inline uint32_t XT_CALLBACK 
ForwardOnBuffer(XtStream const* coreStream, XtBuffer const* coreBuffer, void* user);
inline void XT_CALLBACK 
ForwardOnRunning(XtStream const* coreStream, XtBool running, uint64_t error, void* user);

} // namespace Xt::Detail
#endif // XT_CPP_FORWARD_HPP