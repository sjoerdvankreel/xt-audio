#ifndef XT_AUDIO_STREAM_H
#define XT_AUDIO_STREAM_H

#include <xt/audio/Shared.h>
#include <xt/audio/Structs.h>
#include <cstdint>

#ifdef __cplusplus
extern "C" {
#endif

XT_API XtError XT_CALL 
XtStreamStop(XtStream* s);
XT_API XtError XT_CALL 
XtStreamStart(XtStream* s);
XT_API void XT_CALL 
XtStreamDestroy(XtStream* s);
XT_API const XtFormat* XT_CALL 
XtStreamGetFormat(XtStream const* s);
XT_API XtError XT_CALL 
XtStreamGetFrames(XtStream const* s, int32_t* frames);
XT_API XtError XT_CALL 
XtStreamGetLatency(XtStream const* s, XtLatency* latency);

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // XT_AUDIO_STREAM_H