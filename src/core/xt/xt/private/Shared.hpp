#ifndef XT_PRIVATE_SHARED_HPP
#define XT_PRIVATE_SHARED_HPP

#include <xt/audio/Enums.h>
#include <xt/audio/Shared.h>
#include <xt/audio/Structs.h>
#include <cstdint>

#define XT_STRINGIFY(s) #s
#define XT_FAIL(m) XtiFail(__FILE__, __LINE__, __func__, m)
#define XT_TRACE(m) XtiTrace(__FILE__, __LINE__, __func__, m)
#define XT_ASSERT(c) ((c) || (XT_FAIL("Assertion failed: " #c), 0))

typedef uint32_t XtFault;

bool
XtiCalledOnMainThread();
int32_t
XtiGetPopCount64(uint64_t x);
uint32_t
XtiGetErrorFault(XtError error);
int32_t
XtiGetSampleSize(XtSample sample);
XtError
XtiCreateError(XtSystem system, XtFault fault);
XtServiceError
XtiGetServiceError(XtSystem system, XtFault fault);
void
XtiCopyString(char const* source, char* buffer, int32_t* size);

void
XtiFail(char const* file, int32_t line, char const* fun, char const* msg);
void
XtiTrace(char const* file, int32_t line, char const* fun, char const* msg);

void
XtiDeinterleave(void** dst, void const* src, int32_t frames, int32_t channels, int32_t size);
void
XtiInterleave(void* dst, void const* const* src, int32_t frames, int32_t channels, int32_t size);

#endif // XT_PRIVATE_SHARED_HPP