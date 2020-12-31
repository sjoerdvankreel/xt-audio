#ifndef XT_API_SHARED_H
#define XT_API_SHARED_H

#include <stdint.h>

#ifdef _WIN32
#define XT_CALL __stdcall
#else
#define XT_CALL
#endif // _WIN32
#define XT_CALLBACK XT_CALL

/** @cond */
#ifndef XT_EXPORT
#define XT_API
#elif (defined _WIN32)
#define XT_API __declspec(dllexport)
#else
#define XT_API __attribute__((visibility("default")))
#endif // XT_EXPORT
/** @endcond */

typedef int32_t XtBool;
typedef uint64_t XtError;
static XtBool const XtTrue = 1;
static XtBool const XtFalse = 0;

/** @cond */
typedef struct XtDevice XtDevice;
typedef struct XtStream XtStream;
typedef struct XtService XtService;
typedef struct XtPlatform XtPlatform;
typedef struct XtDeviceList XtDeviceList;
/** @endcond */

#endif // XT_API_SHARED_H