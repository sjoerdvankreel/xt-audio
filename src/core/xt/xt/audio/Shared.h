#ifndef XT_AUDIO_SHARED_H
#define XT_AUDIO_SHARED_H

#include <stdint.h>

#ifdef _WIN32
#define XT_CALL __stdcall
#else
#define XT_CALL
#endif // _WIN32
#define XT_CALLBACK XT_CALL

#ifndef XT_EXPORT
#define XT_API
#elif (defined _WIN32)
#define XT_API __declspec(dllexport)
#else
#define XT_API __attribute__((visibility("default")))
#endif // XT_EXPORT

typedef int32_t XtBool;
typedef uint64_t XtError;
static XtBool const XtTrue = 1;
static XtBool const XtFalse = 0;

typedef struct XtDevice XtDevice;
typedef struct XtStream XtStream;
typedef struct XtService XtService;
typedef struct XtPlatform XtPlatform;

#endif // XT_AUDIO_SHARED_H