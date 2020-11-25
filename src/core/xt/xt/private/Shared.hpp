#ifndef XT_PRIVATE_SHARED_HPP
#define XT_PRIVATE_SHARED_HPP

#include <xt/audio/Enums.h>
#include <xt/audio/Shared.h>
#include <cstdint>

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
char const* 
XtiGetFaultText(XtSystem system, XtFault fault);
XtCause 
XtiGetFaultCause(XtSystem system, XtFault fault);
void
XtiCopyString(char const* source, char* buffer, int32_t* size);

#endif // XT_PRIVATE_SHARED_HPP