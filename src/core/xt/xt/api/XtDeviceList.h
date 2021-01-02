#ifndef XT_API_DEVICE_LIST_H
#define XT_API_DEVICE_LIST_H

/** @cond */
#include <xt/api/Enums.h>
#include <xt/api/Shared.h>
#include <stdint.h>
/** @endcond */

#ifdef __cplusplus
extern "C" {
#endif

XT_API void XT_CALL
XtDeviceListDestroy(XtDeviceList* l);
XT_API XtError XT_CALL
XtDeviceListGetCount(XtDeviceList const* l, int32_t* count);
XT_API XtError XT_CALL
XtDeviceListGetId(XtDeviceList const* l, int32_t index, char* buffer, int32_t* size);
XT_API XtError XT_CALL
XtDeviceListGetName(XtDeviceList const* l, char const* id, char* buffer, int32_t* size);
XT_API XtError XT_CALL 
XtDeviceListGetCapabilities(XtDeviceList const* l, char const* id, XtDeviceCaps* capabilities);

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // XT_API_DEVICE_LIST_H