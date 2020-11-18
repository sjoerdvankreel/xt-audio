#ifndef XT_AUDIO_H
#define XT_AUDIO_H

/** @file */

#include <stdint.h>

#ifdef _WIN32
#define XT_CALL __stdcall
#else
#define XT_CALL
#endif /* _WIN32 */
#define XT_CALLBACK XT_CALL

#ifndef XT_EXPORT
#define XT_API
#elif (defined _WIN32)
#define XT_API __declspec(dllexport)
#else
#define XT_API __attribute__((visibility("default")))
#endif /* XT_EXPORT */

#ifdef __cplusplus
extern "C" {
#endif

typedef int32_t XtBool;
typedef uint64_t XtError;
static const XtBool XtTrue = 1;
static const XtBool XtFalse = 0;

typedef struct XtDevice XtDevice;
typedef struct XtStream XtStream;
typedef struct XtService XtService;

enum XtCause {
  XtCauseFormat,
  XtCauseService,
  XtCauseGeneric,
  XtCauseUnknown,
  XtCauseEndpoint
};

enum XtSystem {
  XtSystemALSA = 1,
  XtSystemASIO,
  XtSystemJACK,
  XtSystemWASAPI,
  XtSystemPulseAudio,
  XtSystemDirectSound
};

enum XtSample {
  XtSampleUInt8,
  XtSampleInt16,
  XtSampleInt24,
  XtSampleInt32,
  XtSampleFloat32
};

enum XtSetup {
  XtSetupProAudio,
  XtSetupSystemAudio,
  XtSetupConsumerAudio
};

enum XtCapabilities {
  XtCapabilitiesNone = 0x0,
  XtCapabilitiesTime = 0x1,
  XtCapabilitiesLatency = 0x2,
  XtCapabilitiesFullDuplex = 0x4,
  XtCapabilitiesChannelMask = 0x8,
  XtCapabilitiesXRunDetection = 0x10
};

typedef enum XtCause XtCause;
typedef enum XtSetup XtSetup;
typedef enum XtSystem XtSystem;
typedef enum XtSample XtSample;
typedef enum XtCapabilities XtCapabilities;

typedef void (XT_CALLBACK *XtOnXRun)(int32_t index, void* user);
typedef void (XT_CALLBACK *XtOnError)(const char* location, const char* message);
typedef void (XT_CALLBACK *XtOnBuffer)(const struct XtStream* stream, const struct XtBuffer* buffer, void* user);

struct XtBuffer {
  const void* input;
  void* output;
  double time;
  uint64_t position;
  XtError error;
  int32_t frames;
  XtBool timeValid;
};

struct XtVersion {
  int32_t major;
  int32_t minor;
};

struct XtErrorInfo {
  XtSystem system;
  XtCause cause;
  const char* text;
  uint32_t fault;
};

struct XtMix {
  int32_t rate;
  XtSample sample;
};

struct XtChannels {
  int32_t inputs;
  uint64_t inMask;
  int32_t outputs;
  uint64_t outMask;
};

struct XtFormat {
  struct XtMix mix;
  struct XtChannels channels;
};

struct XtBufferSize {
  double min;
  double max;
  double current;
};

struct XtLatency {
  double input;
  double output;
};

struct XtAttributes {
  int32_t size;
  int32_t count;
  XtBool isFloat;
  XtBool isSigned;
};

struct XtStreamParams {
  XtBool interleaved;
  XtOnBuffer onBuffer;
  XtOnXRun onXRun;
};

struct XtDeviceStreamParams {
  struct XtStreamParams stream;
  struct XtFormat format;
  double bufferSize;
};

struct XtAggregateDeviceParams {
  XtDevice* device;
  struct XtChannels channels;
  double bufferSize;
};

struct XtAggregateStreamParams {
  struct XtStreamParams stream;
  struct XtAggregateDeviceParams* devices;
  int32_t count;
  struct XtMix mix;
  const XtDevice* master;
};

typedef struct XtMix XtMix;
typedef struct XtBuffer XtBuffer;
typedef struct XtFormat XtFormat;
typedef struct XtVersion XtVersion;
typedef struct XtLatency XtLatency;
typedef struct XtChannels XtChannels;
typedef struct XtErrorInfo XtErrorInfo;
typedef struct XtAttributes XtAttributes;
typedef struct XtBufferSize XtBufferSize;
typedef struct XtStreamParams XtStreamParams;
typedef struct XtDeviceStreamParams XtDeviceStreamParams;
typedef struct XtAggregateStreamParams XtAggregateStreamParams;
typedef struct XtAggregateDeviceParams XtAggregateDeviceParams;

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