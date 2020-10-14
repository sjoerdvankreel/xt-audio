/* Copyright (C) 2015-2020 Sjoerd van Kreel.
 *
 * This file is part of XT-Audio.
 *
 * XT-Audio is free software: you can redistribute it and/or modify it under the 
 * terms of the GNU Lesser General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later version.
 *
 * XT-Audio is distributed in the hope that it will be useful, but WITHOUT ANY 
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with XT-Audio. If not, see<http://www.gnu.org/licenses/>.
 */
#ifndef _XT_AUDIO_H
#define _XT_AUDIO_H

/** @file */

/** @cond */
#include <stdint.h>
/** @endcond */

#ifdef _WIN32
#define XT_CALL __stdcall
#else
#define XT_CALL
#endif /* _WIN32 */
#define XT_CALLBACK XT_CALL

/** @cond */
#ifndef XT_SHARED
#define XT_API
#elif (defined _WIN32)
#define XT_API __declspec(dllexport)
#else
#define XT_API __attribute__((visibility("default")))
#endif /* XT_SHARED */
/** @endcond */

#ifdef __cplusplus
extern "C" {
#endif

typedef int32_t XtBool;
typedef uint64_t XtError;

static const XtBool XtTrue = 1;
static const XtBool XtFalse = 0;

/** @cond */
typedef struct XtDevice XtDevice;
typedef struct XtStream XtStream;
typedef struct XtService XtService;
/** @endcond */

enum XtLevel {
  /** @brief An informational message. */ 
  XtLevelInfo,
  /** @brief An error message. */ 
  XtLevelError,
  /** @brief A fatal error message. */ 
  XtLevelFatal
};

enum XtCause {
  /** @brief Invalid or unsupported audio format. */
  XtCauseFormat,
  /** @brief A required system service is not running or otherwise unavailable. */
  XtCauseService,
  /** @brief A generic, but anticipated, error occurred. */
  XtCauseGeneric,
  /** @brief An unknown error occurred. */
  XtCauseUnknown,
  /** @brief Device or channel disconnected, in use, or otherwise unavailable. */
  XtCauseEndpoint
};

enum XtSystem {
  /** @brief Linux ALSA backend. */
  XtSystemAlsa = 1,
  /** @brief Windows Steinberg ASIO backend. */
  XtSystemAsio,
  /** @brief Linux JACK backend. */
  XtSystemJack,
  /** @brief Linux PulseAudio backend. */
  XtSystemPulse,
  /** @brief Windows DirectSound backend. */
  XtSystemDSound,
  /** @brief Windows WASAPI backend. */
  XtSystemWasapi
};

enum XtSample {
  /** @brief 8-bit unsigned integer. */
  XtSampleUInt8,
  /** @brief 16-bit signed integer. */
  XtSampleInt16,
  /** @brief 24-bit signed integer (packed). */
  XtSampleInt24,
  /** @brief 32-bit signed integer. */
  XtSampleInt32,
  /** @brief 32-bit floating-point. */
  XtSampleFloat32
};

enum XtSetup {
  /** @brief Pro audio setup (ASIO and JACK).
    *
    * Focused on full-duplex operation and low latency.
    * Device sharing and automatic format conversions are less important.
    * May require specialized software and/or hardware.
    */
  XtSetupProAudio,
  /** @brief System audio setup (WASAPI and ALSA).
    *
    * Focused on direct hardware access and low latency.
    * Full duplex operation, device sharing and automatic format conversions are less important.
    * Usually does not require additional software.
    */
  XtSetupSystemAudio,
  /** @brief Consumer audio setup (DirectSound and PulseAudio).
    *
    * Focused on device sharing and automatic format conversions.
    * Full duplex operation, direct hardware access and low latency are less important.
    * Usually does not require additional software.
    */
  XtSetupConsumerAudio
};

enum XtCapabilities {
  /** @brief No capabilities. */
  XtCapabilitiesNone = 0x0,
  /** @brief Time stamping.
    *
    * Audio streams are able to provide accurate position and time
    * stamp information which may be used for stream synchronization.
    * @see XtStreamCallback
    */
  XtCapabilitiesTime = 0x1,
  /** @brief Latency information.
    *
    * Applications can query the end-to-end latency of a running audio stream.
    * @see XtStreamGetLatency
    */
  XtCapabilitiesLatency = 0x2,
  /** @brief Full duplex operation.
    *
    * Devices support streams with both input and output channels.
    * @see XtFormat
    */
  XtCapabilitiesFullDuplex = 0x4,
  /** @brief Channel masking.
    *
    * Applications can route stream channels from/to specific input/
    * output channels, for example to specific speaker positions.
    * @see XtFormat
    */
  XtCapabilitiesChannelMask = 0x8,

  /** @brief Under/overflow detection.
   *
   * Streams support xrun detection and will invoke the 
   * application's xrun callback when an under/overflow occurs.
   *
   * @see XtXRunCallback
   */
  XtCapabilitiesXRunDetection = 0x10
};

/** @cond */
typedef enum XtLevel XtLevel;
typedef enum XtCause XtCause;
typedef enum XtSetup XtSetup;
typedef enum XtSystem XtSystem;
typedef enum XtSample XtSample;
typedef enum XtCapabilities XtCapabilities;
/** @endcond */

struct XtMix {
  /** @brief Sample rate in Hz. */
  int32_t rate;
  /** @brief Sample type. */
  XtSample sample;
};

struct XtChannels {
  /** @brief Input channel count. */
  int32_t inputs;
  /**
    * @brief Input channel mask.
    *
    * Set bit N to route channel N from/to that position. For example,
    * in a stereo stream on a surround71 capable device where channels
    * 4 and 5 represent back left/right, the mask is (1ULL << 4) | (1ULL << 5)
    * to use back left/right instead of the default front left/right.
    */
  uint64_t inMask;
  /** @brief Output channel count. */
  int32_t outputs;
  /**
    * @brief Output channel mask.
    *
    * Set bit N to route channel N from/to that position. For example,
    * in a stereo stream on a surround71 capable device where channels
    * 4 and 5 represent back left/right, the mask is (1ULL << 4) | (1ULL << 5)
    * to use back left/right instead of the default front left/right.
    */
  uint64_t outMask;
};

struct XtFormat {
  /** @brief Sample rate and type. */
  struct XtMix mix;
  /** @brief Channel counts and mappings. */
  struct XtChannels channels;
};

struct XtBuffer {
  /** @brief Minimum size in milliseconds. */
  double min;
  /** @brief Maximum size in milliseconds. */
  double max;
  /** @brief Current or default size in milliseconds. */
  double current;
};

struct XtLatency {
  /** @brief Input latency in milliseconds, or 0 when no inputs are present. */
  double input;
  /** @brief Output latency in milliseconds, or 0 when no outputs are present. */
  double output;
};

struct XtAttributes {
  /** @brief Sample size in bytes. */
  int32_t size;
  /** @brief Indicates whether samples are integer or floating point. */
  XtBool isFloat;
  /** @brief Indicates whether samples are signed or unsigned. */
  XtBool isSigned;
};

/** @cond */
typedef struct XtMix XtMix;
typedef struct XtFormat XtFormat;
typedef struct XtBuffer XtBuffer;
typedef struct XtLatency XtLatency;
typedef struct XtChannels XtChannels;
typedef struct XtAttributes XtAttributes;
/** @endcond */

typedef void (XT_CALLBACK *XtFatalCallback)(void);
typedef void (XT_CALLBACK *XtXRunCallback)(int32_t index, void* user);
typedef void (XT_CALLBACK *XtTraceCallback)(XtLevel level, const char* message);
typedef void (XT_CALLBACK *XtStreamCallback)(
  const XtStream* stream, const void* input, void* output, int32_t frames,
  double time, uint64_t position, XtBool timeValid, XtError error, void* user);

/** 
 * @ingroup audio
 * @{ 
 */
XT_API XtCause XT_CALL XtErrorGetCause(XtError error);
XT_API uint32_t XT_CALL XtErrorGetFault(XtError error);
XT_API XtSystem XT_CALL XtErrorGetSystem(XtError error);
XT_API const char* XT_CALL XtErrorGetText(XtError error);
/** @} */

/** 
 * @ingroup print
 * @{ 
 */
XT_API const char* XT_CALL XtPrintLevelToString(XtLevel level);
XT_API const char* XT_CALL XtPrintCauseToString(XtCause cause);
XT_API const char* XT_CALL XtPrintSetupToString(XtSetup setup);
XT_API const char* XT_CALL XtPrintSystemToString(XtSystem system);
XT_API const char* XT_CALL XtPrintSampleToString(XtSample sample);

XT_API char* XT_CALL XtPrintErrorToString(XtError error);
XT_API char* XT_CALL XtPrintMixToString(const XtMix* mix);
XT_API char* XT_CALL XtPrintFormatToString(const XtFormat* format);
XT_API char* XT_CALL XtPrintBufferToString(const XtBuffer* buffer);
XT_API char* XT_CALL XtPrintLatencyToString(const XtLatency* latency);
XT_API char* XT_CALL XtPrintChannelsToString(const XtChannels* channels);
XT_API char* XT_CALL XtPrintCapabilitiesToString(XtCapabilities capabilities);
XT_API char* XT_CALL XtPrintAttributesToString(const XtAttributes* attributes);
/** @} */

/** 
 * @ingroup audio
 * @{ 
 */
XT_API XtBool XT_CALL XtAudioIsWin32(void);
XT_API void XT_CALL XtAudioTerminate(void);
XT_API void XT_CALL XtAudioFree(void* ptr);
XT_API int32_t XT_CALL XtAudioGetVersionMajor(void);
XT_API int32_t XT_CALL XtAudioGetVersionMinor(void);
XT_API int32_t XT_CALL XtAudioGetServiceCount(void);
XT_API const XtService* XT_CALL XtAudioGetServiceByIndex(int32_t index);
XT_API const XtService* XT_CALL XtAudioGetServiceBySetup(XtSetup setup);
XT_API const XtService* XT_CALL XtAudioGetServiceBySystem(XtSystem system);
XT_API void XT_CALL XtAudioGetSampleAttributes(XtSample sample, XtAttributes* attributes);
XT_API void XT_CALL XtAudioInit(const char* id, void* window, XtTraceCallback trace, XtFatalCallback fatal);
/** @} */

/** 
 * @ingroup service
 * @{ 
 */
XT_API XtSystem XT_CALL XtServiceGetSystem(const XtService* s);
XT_API const char* XT_CALL XtServiceGetName(const XtService* s);
XT_API XtCapabilities XT_CALL XtServiceGetCapabilities(const XtService* s);
XT_API XtError XT_CALL XtServiceGetDeviceCount(const XtService* s, int32_t* count);
XT_API XtError XT_CALL XtServiceOpenDevice(const XtService* s, int32_t index, XtDevice** device);
XT_API XtError XT_CALL XtServiceOpenDefaultDevice(const XtService* s, XtBool output, XtDevice** device);
XT_API XtError XT_CALL XtServiceAggregateStream(const XtService* s, XtDevice** devices, const XtChannels* channels, 
                                                const double* bufferSizes, int32_t count, const XtMix* mix,
                                                XtBool interleaved, XtDevice* master, XtStreamCallback streamCallback, 
                                                XtXRunCallback xRunCallback, void* user, XtStream** stream); 
/** @} */

/** 
 * @ingroup device
 * @{ 
 */
XT_API void XT_CALL XtDeviceDestroy(XtDevice* d);
XT_API XtError XT_CALL XtDeviceShowControlPanel(XtDevice* d);
XT_API XtSystem XT_CALL XtDeviceGetSystem(const XtDevice* d);
XT_API XtError XT_CALL XtDeviceGetMix(const XtDevice* d, XtMix** mix);
XT_API XtError XT_CALL XtDeviceGetName(const XtDevice* d, char** name);
XT_API XtError XT_CALL XtDeviceGetChannelCount(const XtDevice* d, XtBool output, int32_t* count);
XT_API XtError XT_CALL XtDeviceGetBuffer(const XtDevice* d, const XtFormat* format, XtBuffer* buffer);
XT_API XtError XT_CALL XtDeviceSupportsAccess(const XtDevice* d, XtBool interleaved, XtBool* supports);
XT_API XtError XT_CALL XtDeviceSupportsFormat(const XtDevice* d, const XtFormat* format, XtBool* supports);
XT_API XtError XT_CALL XtDeviceGetChannelName(const XtDevice* d, XtBool output, int32_t index, char** name);
XT_API XtError XT_CALL XtDeviceOpenStream(XtDevice* d, const XtFormat* format, XtBool interleaved, double bufferSize, 
                                          XtStreamCallback streamCallback, XtXRunCallback xRunCallback, void* user, XtStream** stream);
/** @} */

/** 
 * @ingroup stream
 * @{ 
 */
XT_API void XT_CALL XtStreamDestroy(XtStream* s);
XT_API XtError XT_CALL XtStreamStop(XtStream* s);
XT_API XtError XT_CALL XtStreamStart(XtStream* s);
XT_API XtSystem XT_CALL XtStreamGetSystem(const XtStream* s);
XT_API XtBool XT_CALL XtStreamIsInterleaved(const XtStream* s);
XT_API const XtFormat* XT_CALL XtStreamGetFormat(const XtStream* s);
XT_API XtError XT_CALL XtStreamGetFrames(const XtStream* s, int32_t* frames);
XT_API XtError XT_CALL XtStreamGetLatency(const XtStream* s, XtLatency* latency);
/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* _XT_AUDIO_H */
