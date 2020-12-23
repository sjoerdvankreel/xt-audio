#ifndef XT_API_DEVICE_H
#define XT_API_DEVICE_H

#include <xt/api/Shared.h>
#include <xt/api/Structs.h>

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

XT_API void XT_CALL 
XtDeviceDestroy(XtDevice* d);
XT_API void* XT_CALL
XtDeviceGetHandle(XtDevice* d);
XT_API XtError XT_CALL 
XtDeviceShowControlPanel(XtDevice* d);
XT_API XtError XT_CALL 
XtDeviceGetMix(XtDevice const* d, XtBool* valid, XtMix* mix);
XT_API XtError XT_CALL 
XtDeviceGetChannelCount(XtDevice const* d, XtBool output, int32_t* count);
XT_API XtError XT_CALL 
XtDeviceSupportsAccess(XtDevice const* d, XtBool interleaved, XtBool* supports);
XT_API XtError XT_CALL 
XtDeviceSupportsFormat(XtDevice const* d, const XtFormat* format, XtBool* supports);
XT_API XtError XT_CALL 
XtDeviceGetBufferSize(XtDevice const* d, const XtFormat* format, XtBufferSize* size);
XT_API XtError XT_CALL 
XtDeviceOpenStream(XtDevice* d, XtDeviceStreamParams const* params, void* user, XtStream** stream);
XT_API XtError XT_CALL 
XtDeviceGetChannelName(XtDevice const* d, XtBool output, int32_t index, char* buffer, int32_t* size);

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // XT_API_DEVICE_H