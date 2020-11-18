#ifndef XT_AUDIO_H
#define XT_AUDIO_H

/** @file */

#include <xt/audio/Enums.h>
#include <xt/audio/Shared.h>
#include <xt/audio/Structs.h>
#include <xt/audio/Callbacks.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

XT_API const char* XT_CALL XtPrintCauseToString(XtCause cause);
XT_API const char* XT_CALL XtPrintSetupToString(XtSetup setup);
XT_API const char* XT_CALL XtPrintSystemToString(XtSystem system);
XT_API const char* XT_CALL XtPrintSampleToString(XtSample sample);
XT_API const char* XT_CALL XtPrintErrorInfoToString(const XtErrorInfo* info);
XT_API const char* XT_CALL XtPrintCapabilitiesToString(XtCapabilities capabilities);

XT_API void XT_CALL XtAudioTerminate(void);
XT_API XtVersion XT_CALL XtAudioGetVersion(void);
XT_API XtSystem XT_CALL XtAudioSetupToSystem(XtSetup setup);
XT_API XtErrorInfo XT_CALL XtAudioGetErrorInfo(XtError error);
XT_API const XtService* XT_CALL XtAudioGetService(XtSystem system);
XT_API void XT_CALL XtAudioGetSystems(XtSystem* buffer, int32_t* size);
XT_API XtAttributes XT_CALL XtAudioGetSampleAttributes(XtSample sample);
XT_API void XT_CALL XtAudioInit(const char* id, void* window, XtOnError onError);

XT_API XtCapabilities XT_CALL XtServiceGetCapabilities(const XtService* s);
XT_API XtError XT_CALL XtServiceGetDeviceCount(const XtService* s, int32_t* count);
XT_API XtError XT_CALL XtServiceOpenDevice(const XtService* s, int32_t index, XtDevice** device);
XT_API XtError XT_CALL XtServiceOpenDefaultDevice(const XtService* s, XtBool output, XtDevice** device);
XT_API XtError XT_CALL XtServiceAggregateStream(const XtService* s, const XtAggregateStreamParams* params, void* user, XtStream** stream); 

XT_API void XT_CALL XtDeviceDestroy(XtDevice* d);
XT_API XtError XT_CALL XtDeviceShowControlPanel(XtDevice* d);
XT_API XtError XT_CALL XtDeviceGetMix(const XtDevice* d, XtBool* valid, XtMix* mix);
XT_API XtError XT_CALL XtDeviceGetName(const XtDevice* d, char* buffer, int32_t* size);
XT_API XtError XT_CALL XtDeviceGetChannelCount(const XtDevice* d, XtBool output, int32_t* count);
XT_API XtError XT_CALL XtDeviceSupportsAccess(const XtDevice* d, XtBool interleaved, XtBool* supports);
XT_API XtError XT_CALL XtDeviceSupportsFormat(const XtDevice* d, const XtFormat* format, XtBool* supports);
XT_API XtError XT_CALL XtDeviceGetBufferSize(const XtDevice* d, const XtFormat* format, XtBufferSize* size);
XT_API XtError XT_CALL XtDeviceOpenStream(XtDevice* d, const XtDeviceStreamParams* params, void* user, XtStream** stream);
XT_API XtError XT_CALL XtDeviceGetChannelName(const XtDevice* d, XtBool output, int32_t index, char* buffer, int32_t* size);

XT_API void XT_CALL XtStreamDestroy(XtStream* s);
XT_API XtError XT_CALL XtStreamStop(XtStream* s);
XT_API XtError XT_CALL XtStreamStart(XtStream* s);
XT_API const XtFormat* XT_CALL XtStreamGetFormat(const XtStream* s);
XT_API XtError XT_CALL XtStreamGetFrames(const XtStream* s, int32_t* frames);
XT_API XtError XT_CALL XtStreamGetLatency(const XtStream* s, XtLatency* latency);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* XT_AUDIO_H */