#ifndef XT_API_PUBLIC_AUDIO_H
#define XT_API_PUBLIC_AUDIO_H

#include <xt/api/public/Enums.h>
#include <xt/api/public/Shared.h>
#include <xt/api/public/Structs.h>
#include <xt/api/public/Callbacks.h>
#include <cstdint>

#ifdef __cplusplus
extern "C" {
#endif

XT_API XtVersion XT_CALL 
XtAudioGetVersion(void);
XT_API XtErrorInfo XT_CALL 
XtAudioGetErrorInfo(XtError error);
XT_API XtSystem XT_CALL
XtAudioSetupToSystem(XtSetup setup);
XT_API XtAttributes XT_CALL 
XtAudioGetSampleAttributes(XtSample sample);
XT_API XtPlatform* XT_CALL
XtAudioInit(char const* id, void* window, XtOnError onError);

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // XT_API_PUBLIC_AUDIO_H