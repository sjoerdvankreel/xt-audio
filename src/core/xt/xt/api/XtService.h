#ifndef XT_API_SERVICE_H
#define XT_API_SERVICE_H

#include <xt/api/Enums.h>
#include <xt/api/Shared.h>
#include <xt/api/Structs.h>

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

XT_API XtServiceCaps XT_CALL 
XtServiceGetCapabilities(XtService const* s);
XT_API XtError XT_CALL
XtServiceOpenDevice(XtService const* s, char const* id, XtDevice** device);
XT_API XtError XT_CALL
XtServiceOpenDeviceList(XtService const* s, XtEnumFlags flags, XtDeviceList** list);
XT_API XtError XT_CALL
XtServiceGetDefaultDeviceId(XtService const* s, XtBool output, XtBool* valid, char* buffer, int32_t* size);
XT_API XtError XT_CALL 
XtServiceAggregateStream(XtService const* s, XtAggregateStreamParams const* params, void* user, XtStream** stream); 

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // XT_API_SERVICE_H