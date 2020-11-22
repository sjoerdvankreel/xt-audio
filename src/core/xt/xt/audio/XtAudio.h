#ifndef XT_AUDIO_AUDIO_H
#define XT_AUDIO_AUDIO_H

#include <xt/audio/Enums.h>
#include <xt/audio/Shared.h>
#include <xt/audio/Structs.h>
#include <xt/audio/Callbacks.h>
#include <cstdint>

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
XtAudioInitPlatform(char const* id, void* window, XtOnError onError);

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // XT_AUDIO_AUDIO_H