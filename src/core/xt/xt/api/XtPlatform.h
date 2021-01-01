#ifndef XT_API_PLATFORM_H
#define XT_API_PLATFORM_H

/** @cond */
#include <xt/api/Enums.h>
#include <xt/api/Shared.h>
#include <stdint.h>
/** @endcond */

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
#endif // XT_API_PLATFORM_H