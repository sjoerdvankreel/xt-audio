#ifndef XT_API_STREAM_H
#define XT_API_STREAM_H

#include <xt/api/Shared.h>
#include <xt/api/Structs.h>

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

XT_API void XT_CALL 
XtStreamStop(XtStream* s);
XT_API XtError XT_CALL 
XtStreamStart(XtStream* s);
XT_API void XT_CALL 
XtStreamDestroy(XtStream* s);
XT_API void* XT_CALL
XtStreamGetHandle(XtStream const* s);
XT_API XtBool XT_CALL
XtStreamIsRunning(XtStream const* s);
XT_API const XtFormat* XT_CALL 
XtStreamGetFormat(XtStream const* s);
XT_API XtError XT_CALL 
XtStreamGetFrames(XtStream const* s, int32_t* frames);
XT_API XtError XT_CALL 
XtStreamGetLatency(XtStream const* s, XtLatency* latency);

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // XT_API_STREAM_H