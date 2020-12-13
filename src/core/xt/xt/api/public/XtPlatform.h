#ifndef XT_API_PUBLIC_PLATFORM_H
#define XT_API_PUBLIC_PLATFORM_H

#include <xt/api/public/Enums.h>
#include <xt/api/public/Shared.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

XT_API void XT_CALL 
XtPlatformDestroy(XtPlatform* p);
XT_API XtService const* XT_CALL 
XtPlatformGetService(XtPlatform const* p, XtSystem system);
XT_API void XT_CALL 
XtPlatformGetSystems(XtPlatform const* p, XtSystem* buffer, int32_t* size);

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // XT_API_PUBLIC_PLATFORM_H