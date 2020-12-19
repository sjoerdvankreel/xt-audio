#ifndef XT_API_CALLBACKS_H
#define XT_API_CALLBACKS_H

#include <xt/api/Shared.h>
#include <stdint.h>

typedef struct XtBuffer XtBuffer;
typedef struct XtLocation XtLocation;

typedef void (XT_CALLBACK
*XtOnError)(XtLocation const* location, char const* message);
typedef void (XT_CALLBACK
*XtOnXRun)(XtStream const* stream, int32_t index, void* user);
typedef void (XT_CALLBACK
*XtOnRunning)(XtStream const* stream, XtBool running, void* user);
typedef uint32_t (XT_CALLBACK
*XtOnBuffer)(XtStream const* stream, XtBuffer const* buffer, void* user);

#endif // XT_API_CALLBACKS_H