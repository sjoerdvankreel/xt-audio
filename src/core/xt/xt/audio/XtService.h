#ifndef XT_AUDIO_SERVICE_H
#define XT_AUDIO_SERVICE_H

#include <xt/audio/Enums.h>
#include <xt/audio/Shared.h>
#include <xt/audio/Structs.h>
#include <cstdint>

#ifdef __cplusplus
extern "C" {
#endif

XT_API XtCapabilities XT_CALL 
XtServiceGetCapabilities(XtService const* s);
XT_API XtError XT_CALL 
XtServiceGetDeviceCount(XtService const* s, int32_t* count);
XT_API XtError XT_CALL 
XtServiceOpenDevice(XtService const* s, int32_t index, XtDevice** device);
XT_API XtError XT_CALL 
XtServiceOpenDefaultDevice(XtService const* s, XtBool output, XtDevice** device);
XT_API XtError XT_CALL 
XtServiceAggregateStream(XtService const* s, XtAggregateStreamParams const* params, void* user, XtStream** stream); 

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // XT_AUDIO_SERVICE_H