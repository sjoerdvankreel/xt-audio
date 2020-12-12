#ifndef XT_API_PUBLIC_SERVICE_H
#define XT_API_PUBLIC_SERVICE_H

#include <xt/api/public/Enums.h>
#include <xt/api/public/Shared.h>
#include <xt/api/public/Structs.h>
#include <cstdint>

#ifdef __cplusplus
extern "C" {
#endif

XT_API XtCapabilities XT_CALL 
XtServiceGetCapabilities(XtService const* s);
XT_API XtError XT_CALL
XtServiceOpenDevice(XtService const* s, char const* id, XtDevice** device);
XT_API XtError XT_CALL
XtServiceOpenDeviceList(XtService const* s, XtEnumFlags flags, XtDeviceList** list);
XT_API XtError XT_CALL 
XtServiceAggregateStream(XtService const* s, XtAggregateStreamParams const* params, void* user, XtStream** stream); 

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // XT_API_PUBLIC_SERVICE_H