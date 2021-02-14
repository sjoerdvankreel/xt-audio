#ifndef XT_API_CALLBACKS_H
#define XT_API_CALLBACKS_H

/** @cond */
#include <xt/api/Shared.h>
#include <stdint.h>
/** @endcond */

/** @cond */
typedef struct XtBuffer XtBuffer;
/** @endcond */

typedef void (XT_CALLBACK
*XtOnError)(char const* message);
typedef void (XT_CALLBACK
*XtOnXRun)(XtStream const* stream, int32_t index, void* user);
typedef uint32_t (XT_CALLBACK
*XtOnBuffer)(XtStream const* stream, XtBuffer const* buffer, void* user);
typedef void (XT_CALLBACK
*XtOnRunning)(XtStream const* stream, XtBool running, XtError error, void* user);

#endif // XT_API_CALLBACKS_H