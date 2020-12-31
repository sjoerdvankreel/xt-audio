#ifndef XT_API_STRUCTS_H
#define XT_API_STRUCTS_H

/** @cond */
#include <xt/api/Enums.h>
#include <xt/api/Shared.h>
#include <xt/api/Callbacks.h>

#include <stdint.h>
/** @endcond */

typedef struct XtMix
{
  int32_t rate;
  XtSample sample;
} XtMix;

typedef struct XtBuffer 
{
  void const* input;
  void* output;
  double time;
  uint64_t position;
  int32_t frames;
  XtBool timeValid;
} XtBuffer;

typedef struct XtVersion 
{
  int32_t major;
  int32_t minor;
} XtVersion;

typedef struct XtLatency 
{
  double input;
  double output;
} XtLatency;

typedef struct XtLocation
{
  char const* file;
  char const* func;
  int32_t line;
} XtLocation;

typedef struct XtChannels 
{
  int32_t inputs;
  uint64_t inMask;
  int32_t outputs;
  uint64_t outMask;
} XtChannels;

typedef struct XtFormat 
{
  XtMix mix;
  XtChannels channels;
} XtFormat;

typedef struct XtServiceError
{
  XtCause cause;
  char const* text;
} XtServiceError;

typedef struct XtErrorInfo 
{
  uint32_t fault;
  XtSystem system;
  XtServiceError service;
} XtErrorInfo;

typedef struct XtBufferSize
{
  double min;
  double max;
  double current;
} XtBufferSize;

typedef struct XtAttributes
{
  int32_t size;
  int32_t count;
  XtBool isFloat;
  XtBool isSigned;
} XtAttributes;

typedef struct XtStreamParams
{
  XtBool interleaved;
  XtOnBuffer onBuffer;
  XtOnXRun onXRun;
  XtOnRunning onRunning;
} XtStreamParams;

typedef struct XtDeviceStreamParams 
{
  XtStreamParams stream;
  XtFormat format;
  double bufferSize;
} XtDeviceStreamParams;

typedef struct XtAggregateDeviceParams 
{
  XtDevice* device;
  XtChannels channels;
  double bufferSize;
} XtAggregateDeviceParams;

typedef struct XtAggregateStreamParams 
{
  XtStreamParams stream;
  XtAggregateDeviceParams* devices;
  int32_t count;
  XtMix mix;
  XtDevice const* master;
} XtAggregateStreamParams;

#endif // XT_API_STRUCTS_H