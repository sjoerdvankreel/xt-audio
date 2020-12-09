#ifndef XT_API_PUBLIC_CALLBACKS_H
#define XT_API_PUBLIC_CALLBACKS_H

#include <xt/api/public/Shared.h>
#include <cstdint>

typedef void (XT_CALLBACK
*XtOnXRun)(int32_t index, void* user);
typedef void (XT_CALLBACK
*XtOnError)(struct XtLocation const* location, char const* message);
typedef void (XT_CALLBACK
*XtOnBuffer)(XtStream const* stream, struct XtBuffer const* buffer, void* user);

#endif // XT_API_PUBLIC_CALLBACKS_H