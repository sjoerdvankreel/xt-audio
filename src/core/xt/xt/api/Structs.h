#ifndef XT_API_STRUCTS_H
#define XT_API_STRUCTS_H

/** @cond */
#include <xt/api/Enums.h>
#include <xt/api/Shared.h>
#include <xt/api/Callbacks.h>

#include <stdint.h>
/** @endcond */

/** @cond */
typedef struct XtMix XtMix;
typedef struct XtFormat XtFormat;
typedef struct XtBuffer XtBuffer; 
typedef struct XtVersion XtVersion; 
typedef struct XtLatency XtLatency; 
typedef struct XtChannels XtChannels; 
typedef struct XtErrorInfo XtErrorInfo; 
typedef struct XtBufferSize XtBufferSize;
typedef struct XtAttributes XtAttributes;
typedef struct XtServiceError XtServiceError;
typedef struct XtStreamParams XtStreamParams;
typedef struct XtDeviceStreamParams XtDeviceStreamParams; 
typedef struct XtAggregateDeviceParams XtAggregateDeviceParams;
typedef struct XtAggregateStreamParams XtAggregateStreamParams;
/** @endcond */

struct XtMix
{
  int32_t rate;
  XtSample sample;
};

struct XtBuffer 
{
  void const* input;
  void* output;
  double time;
  uint64_t position;
  int32_t frames;
  XtBool timeValid;
};

struct XtVersion 
{
  int32_t major;
  int32_t minor;
};

struct XtLatency 
{
  double input;
  double output;
};

struct XtChannels 
{
  int32_t inputs;
  uint64_t inMask;
  int32_t outputs;
  uint64_t outMask;
};

struct XtFormat 
{
  XtMix mix;
  XtChannels channels;
};

struct XtServiceError
{
  XtCause cause;
  char const* text;
};

struct XtErrorInfo 
{
  uint32_t fault;
  XtSystem system;
  XtServiceError service;
};

struct XtBufferSize
{
  double min;
  double max;
  double current;
};

struct XtAttributes
{
  int32_t size;
  int32_t count;
  XtBool isFloat;
  XtBool isSigned;
};

struct XtStreamParams
{
  XtBool interleaved;
  XtOnBuffer onBuffer;
  XtOnXRun onXRun;
  XtOnRunning onRunning;
};

struct XtDeviceStreamParams 
{
  XtStreamParams stream;
  XtFormat format;
  double bufferSize;
};

struct XtAggregateDeviceParams 
{
  XtDevice* device;
  XtChannels channels;
  double bufferSize;
};

struct XtAggregateStreamParams 
{
  XtStreamParams stream;
  XtAggregateDeviceParams* devices;
  int32_t count;
  XtMix mix;
  XtDevice const* master;
};

#endif // XT_API_STRUCTS_H