#ifndef XT_AUDIO_PLATFORM_H
#define XT_AUDIO_PLATFORM_H

#include <xt/audio/Enums.h>
#include <xt/audio/Shared.h>
#include <cstdint>

#ifdef __cplusplus
extern "C" {
#endif

XT_API void XT_CALL 
XtPlatformDestroy(XtPlatform* p);
XT_API XtService const* XT_CALL 
XtPlatformGetService(XtPlatform const* p, XtSystem system);
XT_API XtSystem XT_CALL 
XtPlatformSetupToSystem(XtPlatform const* p, XtSetup setup);
XT_API void XT_CALL 
XtPlatformGetSystems(XtPlatform const* p, XtSystem* buffer, int32_t* size);

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // XT_AUDIO_PLATFORM_H