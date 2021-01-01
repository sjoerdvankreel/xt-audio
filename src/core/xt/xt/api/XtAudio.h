#ifndef XT_API_AUDIO_H
#define XT_API_AUDIO_H

/** @cond */
#include <xt/api/Enums.h>
#include <xt/api/Shared.h>
#include <xt/api/Structs.h>
#include <xt/api/Callbacks.h>
#include <stdint.h>
/** @endcond */

#ifdef __cplusplus
extern "C" {
#endif

XT_API XtVersion XT_CALL 
XtAudioGetVersion(void);
XT_API XtErrorInfo XT_CALL 
XtAudioGetErrorInfo(XtError error);
XT_API XtAttributes XT_CALL 
XtAudioGetSampleAttributes(XtSample sample);
XT_API XtPlatform* XT_CALL
XtAudioInit(char const* id, void* window, XtOnError onError);

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // XT_API_AUDIO_H