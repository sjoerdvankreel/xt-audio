// Don't warn about posix-compliant strdup() and friends.
#ifndef _CRT_NONSTDC_NO_DEPRECATE
#define _CRT_NONSTDC_NO_DEPRECATE 1
#endif

#include "xt-private.hpp"
#include <inttypes.h>
#include <sstream>
#include <iomanip>

// ---- local ----

static void InitStreamBuffers(
  XtIntermediateBuffers& buffers, XtBool interleaved, XtBool nonInterleaved,
  const XtFormat* format, int32_t frames, int32_t sampleSize) {

  if(interleaved) {
    buffers.inputInterleaved = std::vector<char>(frames * format->channels.inputs * sampleSize, '\0');
    buffers.outputInterleaved = std::vector<char>(frames * format->channels.outputs * sampleSize, '\0');
  }

  if(nonInterleaved) {
    buffers.inputNonInterleaved = std::vector<void*>(format->channels.inputs, nullptr);
    buffers.outputNonInterleaved = std::vector<void*>(format->channels.outputs, nullptr);
    buffers.inputChannelsNonInterleaved = std::vector<std::vector<char>>(
      format->channels.inputs, std::vector<char>(frames * sampleSize, '\0'));
    buffers.outputChannelsNonInterleaved = std::vector<std::vector<char>>(
      format->channels.outputs, std::vector<char>(frames * sampleSize, '\0'));
    for(int32_t i = 0; i < format->channels.inputs; i++)
      buffers.inputNonInterleaved[i] = &(buffers.inputChannelsNonInterleaved[i][0]);
    for(int32_t i = 0; i < format->channels.outputs; i++)
      buffers.outputNonInterleaved[i] = &(buffers.outputChannelsNonInterleaved[i][0]);
  }
}

static XtError OpenStreamInternal(XtDevice* d, const XtFormat* format, XtBool interleaved, double bufferSize, bool secondary,
                                  XtStreamCallback streamCallback, XtXRunCallback xRunCallback, void* user, XtStream** stream) {

  XtError error;
  XtFault fault;
  int32_t frames;
  XtBool supports;
  XtSystem system;
  XtBool canInterleaved;
  XtBool initInterleaved;
  XtBool canNonInterleaved;
  XtBool initNonInterleaved;

  XT_ASSERT(d != nullptr);
  XT_ASSERT(bufferSize > 0.0);
  XT_ASSERT(format != nullptr);
  XT_ASSERT(stream != nullptr);
  XT_ASSERT(XtiCalledOnMainThread());
  XT_ASSERT(streamCallback != nullptr);
  XT_ASSERT(XtiValidateFormat(d->GetSystem(), *format));

  double rate = format->mix.rate;
  uint64_t inMask = format->channels.inMask;
  int32_t inputs = format->channels.inputs;
  uint64_t outMask = format->channels.outMask;
  int32_t outputs = format->channels.outputs;
  XtSample sample = format->mix.sample;

  auto attributes = XtAudioGetSampleAttributes(sample);

  *stream = nullptr;
  system = XtDeviceGetSystem(d);  
  if((error = XtDeviceSupportsFormat(d, format, &supports)) != 0)
    return error;
  if(!supports)
    return XtiCreateError(system, XtAudioGetServiceBySystem(system)->GetFormatFault());
  if((error = XtDeviceSupportsAccess(d, XtTrue, &canInterleaved)) != 0)
    return error;
  if((error = XtDeviceSupportsAccess(d, XtFalse, &canNonInterleaved)) != 0)
    return error;
  if((fault = d->OpenStream(format, interleaved, bufferSize, secondary, streamCallback, user, stream)) != 0)
    return XtiCreateError(d->GetSystem(), fault);
  if((fault = (*stream)->GetFrames(&frames)) != 0) {
    XtStreamDestroy(*stream);
    return XtiCreateError(d->GetSystem(), fault);
  }

  (*stream)->user = user;
  (*stream)->format = *format;
  (*stream)->aggregated = false;
  (*stream)->aggregationIndex = 0;
  (*stream)->interleaved = interleaved;
  (*stream)->xRunCallback = xRunCallback;
  (*stream)->sampleSize = attributes.size;
  (*stream)->streamCallback = streamCallback;
  (*stream)->canInterleaved = canInterleaved;
  (*stream)->canNonInterleaved = canNonInterleaved;
  initInterleaved =  interleaved && !canInterleaved;
  initNonInterleaved = !interleaved && !canNonInterleaved;
  InitStreamBuffers((*stream)->intermediate, initInterleaved, initNonInterleaved, format, frames, attributes.size);
  return 0;
}

// ---- print ----

const char* XT_CALL XtPrintLevelToString(XtLevel level) {
  switch(level) {
  case XtLevelInfo: return "Info";
  case XtLevelError: return "Error";
  case XtLevelFatal: return "Fatal";
  default: return "<Unknown level>";
  }
}

const char* XT_CALL XtPrintCauseToString(XtCause cause) {
  switch(cause) {
  case XtCauseFormat: return "Format";
  case XtCauseGeneric: return "Generic";
  case XtCauseService: return "Service";
  case XtCauseUnknown: return "Unknown";
  case XtCauseEndpoint: return "Endpoint";
  default: return "<Unknown cause>";
  }
}

const char* XT_CALL XtPrintSetupToString(XtSetup setup) {
  switch(setup) {
  case XtSetupProAudio: return "ProAudio";
  case XtSetupSystemAudio: return "SystemAudio";
  case XtSetupConsumerAudio: return "ConsumerAudio";
  default: return "<Unknown setup>";
  }
}

const char* XT_CALL XtPrintSystemToString(XtSystem system) {
  switch(system) {
  case XtSystemAlsa: return "Alsa";
  case XtSystemAsio: return "Asio";
  case XtSystemJack: return "Jack";
  case XtSystemPulse: return "Pulse";
  case XtSystemDSound: return "DSound";
  case XtSystemWasapi: return "Wasapi";
  default: return "<Unknown system>";
  }
}

const char* XT_CALL XtPrintSampleToString(XtSample sample) {
  switch(sample) {
  case XtSampleUInt8: return "UInt8";
  case XtSampleInt16: return "Int16";
  case XtSampleInt24: return "Int24";
  case XtSampleInt32: return "Int32";
  case XtSampleFloat32: return "Float32";
  default: return "<Unknown sample>";
  }
}

const char** XT_CALL XtPrintCapabilitiesToString(XtCapabilities capabilities) {
  size_t i = 0;
  static thread_local const char* result[6];
  static const char* const allCapabilities[] = { "Time", "Latency", "FullDuplex", "ChannelMask", "XRunDetection" };
  std::memset(result, 0, sizeof(result));
  if((capabilities & XtCapabilitiesTime) != 0) result[i++] = allCapabilities[0];
  if((capabilities & XtCapabilitiesLatency) != 0) result[i++] = allCapabilities[1];
  if((capabilities & XtCapabilitiesFullDuplex) != 0) result[i++] = allCapabilities[2];
  if((capabilities & XtCapabilitiesChannelMask) != 0) result[i++] = allCapabilities[3];
  if((capabilities & XtCapabilitiesXRunDetection) != 0) result[i++] = allCapabilities[4];
  return result;
}

// ---- audio ----

void XT_CALL XtAudioFree(void* ptr) {
  free(ptr);
}

int32_t XT_CALL XtAudioGetVersionMajor(void) {
  return 1;
}

int32_t XT_CALL XtAudioGetVersionMinor(void) {
  return 7;
}

uint32_t XT_CALL XtAudioGetErrorFault(XtError error) {
  return static_cast<XtFault>(error & 0x00000000FFFFFFFF);
}

XtSystem XT_CALL XtAudioGetErrorSystem(XtError error) {
  XT_ASSERT(error != 0);
  return static_cast<XtSystem>((error & 0xFFFFFFFF00000000) >> 32ULL);
}

const char* XT_CALL XtAudioGetErrorText(XtError error) {
  XT_ASSERT(error != 0);
  return XtAudioGetServiceBySystem(XtAudioGetErrorSystem(error))->GetFaultText(XtAudioGetErrorFault(error));
}

XtCause XT_CALL XtAudioGetErrorCause(XtError error) {
  XT_ASSERT(error != 0);
  return XtAudioGetServiceBySystem(XtAudioGetErrorSystem(error))->GetFaultCause(XtAudioGetErrorFault(error));
}

const XtService* XT_CALL XtAudioGetServiceBySetup(XtSetup setup) {
  XT_ASSERT(XtSetupProAudio <= setup && setup <= XtSetupConsumerAudio);
  return XtAudioGetServiceBySystem(XtiSetupToSystem(setup));
}

XtAttributes XT_CALL XtAudioGetSampleAttributes(XtSample sample) {
  XT_ASSERT(XtSampleUInt8 <= sample && sample <= XtSampleFloat32);
  XtAttributes result;
  result.isSigned = sample != XtSampleUInt8;
  result.isFloat = sample == XtSampleFloat32;
  switch(sample) {
  case XtSampleUInt8: result.size = 1; break;
  case XtSampleInt16: result.size = 2; break;
  case XtSampleInt24: result.size = 3; break;
  case XtSampleInt32: result.size = 4; break;
  case XtSampleFloat32: result.size = 4; break;
  default: XT_FAIL("Unknown sample"); break;
  }
  return result;
}

void XT_CALL XtAudioTerminate(void) {
  XT_ASSERT(XtiCalledOnMainThread());
  XT_TRACE(XtLevelInfo, "Terminating library (version %d.%d)...", XtAudioGetVersionMajor(), XtAudioGetVersionMinor());
  XtiTerminatePlatform();
  XT_TRACE(XtLevelInfo, "Terminated library (version %d.%d).", XtAudioGetVersionMajor(), XtAudioGetVersionMinor());
  free(XtiId);
  XtiId = nullptr;
  XtiTraceCallback = nullptr;
  XtiFatalCallback = nullptr;
}

void XT_CALL XtAudioInit(const char* id, void* window, XtTraceCallback trace, XtFatalCallback fatal) {
  XtiTraceCallback = trace;
  XtiFatalCallback = fatal;
  XtiId = id == nullptr || strlen(id) == 0? strdup("XT-Audio"): strdup(id);
  XT_TRACE(XtLevelInfo, "Initializing library (version %d.%d, built %s %s) ...", XtAudioGetVersionMajor(), XtAudioGetVersionMinor(), __DATE__, __TIME__);
  XtiInitPlatform(window);
  XT_TRACE(XtLevelInfo, "Initialized library (version %d.%d).", XtAudioGetVersionMajor(), XtAudioGetVersionMinor());
}

// ---- service ----

XtSystem XT_CALL XtServiceGetSystem(const XtService* s) {
  XT_ASSERT(s != nullptr);
  return s->GetSystem();
}

const char* XT_CALL XtServiceGetName(const XtService* s) { 
  XT_ASSERT(s != nullptr);
  return s->GetName(); 
}

XtCapabilities XT_CALL XtServiceGetCapabilities(const XtService* s) {
  XT_ASSERT(s != nullptr);
  return s->GetCapabilities();
}

XtError XT_CALL XtServiceGetDeviceCount(const XtService* s, int32_t* count) {
  XT_ASSERT(s != nullptr);
  XT_ASSERT(count != nullptr);
  XT_ASSERT(XtiCalledOnMainThread());
  *count = 0;
  return XtiCreateError(s->GetSystem(), s->GetDeviceCount(count));
}

XtError XT_CALL XtServiceOpenDevice(const XtService* s, int32_t index, XtDevice** device) {
  XT_ASSERT(index >= 0);
  XT_ASSERT(s != nullptr);
  XT_ASSERT(device != nullptr);
  XT_ASSERT(XtiCalledOnMainThread());

  *device = nullptr;
  const char* format = "Opening device: service %s, index %d...";
  XT_TRACE(XtLevelInfo, format, XtServiceGetName(s), index);
  XtFault fault = s->OpenDevice(index, device);
  if(fault != 0)
    return XtiCreateError(s->GetSystem(), fault);
  format = "Opened device %s: service %s, index %d.";
  std::string name = XtiTryGetDeviceName(*device);
  XT_TRACE(XtLevelInfo, format, name.c_str(), XtServiceGetName(s), index);
  return 0;
}

XtError XT_CALL XtServiceOpenDefaultDevice(const XtService* s, XtBool output, XtDevice** device) {
  XT_ASSERT(s != nullptr);
  XT_ASSERT(device != nullptr);
  XT_ASSERT(XtiCalledOnMainThread());

  *device = nullptr;
  const char* format = "Opening default device: service %s, output %d...";
  XT_TRACE(XtLevelInfo, format, XtServiceGetName(s), output);
  XtFault fault =  s->OpenDefaultDevice(output, device);
  if(fault != 0)
    return XtiCreateError(s->GetSystem(), fault);
  if(*device == nullptr) {
    format = "Service %s: no default device found (output: %d).";
    XT_TRACE(XtLevelInfo, format, XtServiceGetName(s), output);
    return 0;
  }
  format = "Opened default device %s: service %s, output %d.";
  XT_TRACE(XtLevelInfo, format, XtiTryGetDeviceName(*device).c_str(), XtServiceGetName(s), output);
  return 0;
}

XtError XT_CALL XtServiceAggregateStream(const XtService* s, XtDevice** devices, const XtChannels* channels, 
                                         const double* bufferSizes, int32_t count, const XtMix* mix,
                                         XtBool interleaved, XtDevice* master, XtStreamCallback streamCallback, 
                                         XtXRunCallback xRunCallback, void* user, XtStream** stream) {

  XT_ASSERT(count > 0);
  XT_ASSERT(s != nullptr);
  XT_ASSERT(mix != nullptr);
  XT_ASSERT(stream != nullptr);
  XT_ASSERT(master != nullptr);
  XT_ASSERT(devices != nullptr);
  XT_ASSERT(channels != nullptr);
  XT_ASSERT(bufferSizes != nullptr);
  XT_ASSERT(XtiCalledOnMainThread());
  XT_ASSERT(streamCallback != nullptr);

  const char* fmt = "Opening aggregate: service %s, count %d...";
  XT_TRACE(XtLevelInfo, fmt, XtServiceGetName(s), count);

  XtSystem system = XtServiceGetSystem(s);
  auto attrs = XtAudioGetSampleAttributes(mix->sample);
  std::unique_ptr<XtAggregate> result(new XtAggregate);
  result->user = user;
  result->running = 0;
  result->system = system;
  result->masterIndex = -1;
  result->aggregated = false;
  result->aggregationIndex = -1;
  result->insideCallbackCount = 0;
  result->sampleSize = attrs.size;
  result->interleaved = interleaved;
  result->xRunCallback = xRunCallback;
  result->canInterleaved = interleaved;
  result->streamCallback = streamCallback;
  result->canNonInterleaved = !interleaved;
  result->inputRings = std::vector<XtRingBuffer>(count, XtRingBuffer());
  result->outputRings = std::vector<XtRingBuffer>(count, XtRingBuffer());
  result->contexts = std::vector<XtAggregateContext>(count, XtAggregateContext());

  XtError error;
  int32_t frames = 0;
  int32_t thisFrames;
  XtStream* thisStream;
  XtFormat format = { 0 };
  bool masterFound = false;
  XtFormat thisFormat = { 0 };
  XtStreamCallback thisCallback;

  format.mix = *mix;
  for(int32_t i = 0; i < count; i++) {
    thisFormat.mix = *mix;
    thisFormat.channels.inputs = channels[i].inputs;
    thisFormat.channels.inMask = channels[i].inMask;
    thisFormat.channels.outputs = channels[i].outputs;
    thisFormat.channels.outMask = channels[i].outMask;

    result->contexts[i].index = i;
    result->contexts[i].stream = result.get();
    result->channels.push_back(channels[i]);
    format.channels.inputs += channels[i].inputs;
    format.channels.outputs += channels[i].outputs;

    masterFound |= master == devices[i];
    if(master == devices[i])
      result->masterIndex = i;
    thisCallback = master == devices[i]? XtiMasterCallback: XtiSlaveCallback;
    if((error = OpenStreamInternal(devices[i], &thisFormat, interleaved, bufferSizes[i], 
      master != devices[i], thisCallback, xRunCallback, &result->contexts[i], &thisStream)) != 0)
      return error;
    result->streams.push_back(std::unique_ptr<XtStream>(thisStream));
    thisStream->aggregated = true;
    thisStream->aggregationIndex = i;
    if((error = XtStreamGetFrames(thisStream, &thisFrames)) != 0)
      return error;
    frames = thisFrames > frames? thisFrames: frames;
  }
  XT_ASSERT(masterFound);

  result->format = format;
  result->frames = frames * 2;
  InitStreamBuffers(result->weave, interleaved, !interleaved, &format, frames, attrs.size);
  InitStreamBuffers(result->intermediate, interleaved, !interleaved, &format, frames, attrs.size);
  for(int32_t i = 0; i < count; i++) {
    result->inputRings[i] = XtRingBuffer(interleaved != XtFalse, result->frames, channels[i].inputs, attrs.size);
    result->outputRings[i] = XtRingBuffer(interleaved != XtFalse, result->frames, channels[i].outputs, attrs.size);
  }
  
  fmt = "Opened aggregate: service %s, count %d...";
  XT_TRACE(XtLevelInfo, fmt, XtServiceGetName(s), count);

  *stream = result.release();
  return 0;
}

// ---- device ----

void XT_CALL XtDeviceDestroy(XtDevice* d) {
  XT_ASSERT(XtiCalledOnMainThread());
  if(d == nullptr)
    return;
  std::string name = XtiTryGetDeviceName(d);
  XT_TRACE(XtLevelInfo, "Closing device %s...", name.c_str());
  delete d; 
  XT_TRACE(XtLevelInfo, "Closed device %s.", name.c_str());
}

XtSystem XT_CALL XtDeviceGetSystem(const XtDevice* d) {
  XT_ASSERT(d != nullptr);
  return d->GetSystem();
}

XtError XT_CALL XtDeviceShowControlPanel(XtDevice* d) {
  XT_ASSERT(d != nullptr);
  XT_ASSERT(XtiCalledOnMainThread());
  return XtiCreateError(d->GetSystem(), d->ShowControlPanel());
}

XtError XT_CALL XtDeviceGetMix(const XtDevice* d, XtMix** mix) {
  XT_ASSERT(d != nullptr);
  XT_ASSERT(mix != nullptr);
  XT_ASSERT(XtiCalledOnMainThread());
  *mix = nullptr;
  return XtiCreateError(d->GetSystem(), d->GetMix(mix));
}

XtError XT_CALL XtDeviceGetName(const XtDevice* d, char** name) {
  XT_ASSERT(d != nullptr);
  XT_ASSERT(name != nullptr);
  XT_ASSERT(XtiCalledOnMainThread());
  *name = nullptr;
  return XtiCreateError(d->GetSystem(), d->GetName(name));
}

XtError XT_CALL XtDeviceGetChannelCount(const XtDevice* d, XtBool output, int32_t* count) {
  XT_ASSERT(d != nullptr);
  XT_ASSERT(count != nullptr);
  XT_ASSERT(XtiCalledOnMainThread());
  *count = 0;
  return XtiCreateError(d->GetSystem(), d->GetChannelCount(output, count));
}

XtError XT_CALL XtDeviceSupportsAccess(const XtDevice* d, XtBool interleaved, XtBool* supports) {
  XT_ASSERT(d != nullptr);
  XT_ASSERT(supports != nullptr);
  XT_ASSERT(XtiCalledOnMainThread());
  *supports = XtFalse;
  return XtiCreateError(d->GetSystem(), d->SupportsAccess(interleaved, supports));
}

XtError XT_CALL XtDeviceGetBuffer(const XtDevice* d, const XtFormat* format, XtBuffer* buffer) {
  XtError error;
  XtBool supports;
  XtSystem system;
  XT_ASSERT(d != nullptr);
  XT_ASSERT(buffer != nullptr);
  XT_ASSERT(format != nullptr);
  XT_ASSERT(XtiCalledOnMainThread());
  XT_ASSERT(XtiValidateFormat(d->GetSystem(), *format));

  system = XtDeviceGetSystem(d);
  memset(buffer, 0, sizeof(XtBuffer));
  if((error = XtDeviceSupportsFormat(d, format, &supports)) != 0)
    return error;
  if(!supports)
    return XtiCreateError(system, XtAudioGetServiceBySystem(system)->GetFormatFault());
  return XtiCreateError(d->GetSystem(), d->GetBuffer(format, buffer));
}

XtError XT_CALL XtDeviceGetChannelName(const XtDevice* d, XtBool output, int32_t index, char** name) {
  XT_ASSERT(index >= 0);
  XT_ASSERT(d != nullptr);
  XT_ASSERT(name != nullptr);
  XT_ASSERT(XtiCalledOnMainThread());
  *name = nullptr;
  return XtiCreateError(d->GetSystem(), d->GetChannelName(output, index, name));
}

XtError XT_CALL XtDeviceSupportsFormat(const XtDevice* d, const XtFormat* format, XtBool* supports) {
  XT_ASSERT(d != nullptr);
  XT_ASSERT(format != nullptr);
  XT_ASSERT(supports != nullptr);
  XT_ASSERT(XtiCalledOnMainThread());
  XT_ASSERT(XtiValidateFormat(d->GetSystem(), *format));
  *supports = XtFalse;
  return XtiCreateError(d->GetSystem(), d->SupportsFormat(format, supports));
}

XtError XT_CALL XtDeviceOpenStream(XtDevice* d, const XtFormat* format, XtBool interleaved, double bufferSize, 
                                   XtStreamCallback streamCallback, XtXRunCallback xRunCallback, void* user, XtStream** stream) {

  return OpenStreamInternal(d, format, interleaved, bufferSize, false, streamCallback, xRunCallback, user, stream);
}

// ---- stream ----

void XT_CALL XtStreamDestroy(XtStream* s) { 
  XT_ASSERT(XtiCalledOnMainThread());
  if(s == nullptr)
    return;
  const char* system = XtPrintSystemToString(XtStreamGetSystem(s));
  XT_TRACE(XtLevelInfo, "Closing stream on system %s...", system);
  delete s;
  XT_TRACE(XtLevelInfo, "Closed stream on system %s.", system);
}

XtError XT_CALL XtStreamStop(XtStream* s) {
  XT_ASSERT(s != nullptr);
  XT_ASSERT(XtiCalledOnMainThread());
  const char* system = XtPrintSystemToString(XtStreamGetSystem(s));
  XT_TRACE(XtLevelInfo, "Stopping stream on system %s...", system);
  XtError error =  XtiCreateError(s->GetSystem(), s->Stop());
  XT_TRACE(XtLevelInfo, "Stopped stream on system %s.", system);
  return error;
}

XtError XT_CALL XtStreamStart(XtStream* s) {
  XT_ASSERT(s != nullptr);
  XT_ASSERT(XtiCalledOnMainThread());
  const char* system = XtPrintSystemToString(XtStreamGetSystem(s));
  XT_TRACE(XtLevelInfo, "Starting stream on system %s...", system);
  XtError error =  XtiCreateError(s->GetSystem(), s->Start());
  XT_TRACE(XtLevelInfo, "Started stream on system %s.", system);
  return error;
}

XtSystem XT_CALL XtStreamGetSystem(const XtStream* s) {
  XT_ASSERT(s != nullptr);
  return s->GetSystem();
}

XtBool XT_CALL XtStreamIsInterleaved(const XtStream* s) {
  XT_ASSERT(s != nullptr);
  return s->interleaved;
}

const XtFormat* XT_CALL XtStreamGetFormat(const XtStream* s) {
  XT_ASSERT(s != nullptr);
  return &s->format;
}

XtError XT_CALL XtStreamGetFrames(const XtStream* s, int32_t* frames) {
  XT_ASSERT(s != nullptr);
  XT_ASSERT(frames != nullptr);
  *frames = 0;
  return XtiCreateError(s->GetSystem(), s->GetFrames(frames));
}

XtError XT_CALL XtStreamGetLatency(const XtStream* s, XtLatency* latency) {
  XT_ASSERT(s != nullptr);
  XT_ASSERT(latency != nullptr);
  memset(latency, 0, sizeof(XtLatency));
  return XtiCreateError(s->GetSystem(), s->GetLatency(latency));
}