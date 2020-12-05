// Don't warn about posix-compliant strdup() and friends.
#ifndef _CRT_NONSTDC_NO_DEPRECATE
#define _CRT_NONSTDC_NO_DEPRECATE 1
#endif

#include <xt/api/public/XtAudio.h>
#include <xt/api/public/XtDevice.h>
#include <xt/api/public/XtStream.h>
#include <xt/api/public/XtService.h>
#include <xt/api/private/Device.hpp>
#include <xt/api/private/Stream.hpp>
#include <xt/api/private/Service.hpp>
#include <xt/api/public/Shared.h>
#include <xt/api/public/Structs.h>
#include <xt/private/Shared.hpp>
#include <xt/private/Structs.hpp>
#include <xt/private/AggregateStream.hpp>
#include <sstream>
#include <cstring>
#include <iomanip>
#include <algorithm>
#include <inttypes.h>

// ---- local ----
// ---- service ----

XtError XT_CALL XtServiceAggregateStream(const XtService* s, const XtAggregateStreamParams* params, void* user, XtStream** stream) {

  XT_ASSERT(s != nullptr);
  XT_ASSERT(params != nullptr);
  XT_ASSERT(params->count > 0);
  XT_ASSERT(stream != nullptr);
  XT_ASSERT(XtiCalledOnMainThread());
  XT_ASSERT(params->master != nullptr);
  XT_ASSERT(params->devices != nullptr);
  XT_ASSERT(params->stream.onBuffer != nullptr);
  XT_ASSERT((s->GetCapabilities() & XtCapabilitiesAggregation) != 0);

  XtSystem system = s->GetSystem();
  auto attrs = XtAudioGetSampleAttributes(params->mix.sample);
  std::unique_ptr<XtAggregateStream> result(new XtAggregateStream);
  result->_user = user;
  result->_running = 0;
  result->_system = system;
  result->_masterIndex = -1;
  result->_insideCallback.store(0);
  result->_params.stream = params->stream;
  result->_emulated = false;
  result->_rings = std::vector<XtIORingBuffers>(params->count, XtIORingBuffers());
  result->_contexts = std::vector<XtAggregateContext>(params->count, XtAggregateContext());

  XtError error;
  int32_t frames = 0;
  int32_t thisFrames;
  XtStream* thisStream;
  XtFormat format = { 0 };
  bool masterFound = false;
  XtFormat thisFormat = { 0 };
  XtOnBuffer onThisBuffer;

  format.mix = params->mix;
  for(int32_t i = 0; i < params->count; i++) {
    thisFormat.mix = params->mix;
    thisFormat.channels.inputs = params->devices[i].channels.inputs;
    thisFormat.channels.inMask = params->devices[i].channels.inMask;
    thisFormat.channels.outputs = params->devices[i].channels.outputs;
    thisFormat.channels.outMask = params->devices[i].channels.outMask;

    result->_contexts[i].index = i;
    result->_contexts[i].stream = result.get();
    result->_channels.push_back(params->devices[i].channels);
    format.channels.inputs += params->devices[i].channels.inputs;
    format.channels.outputs += params->devices[i].channels.outputs;

    masterFound |= params->master == params->devices[i].device;
    if(params->master == params->devices[i].device)
      result->_masterIndex = i;
    onThisBuffer = params->master == params->devices[i].device? XtAggregateStream::OnMasterBuffer: XtAggregateStream::OnSlaveBuffer;
    XtDeviceStreamParams thisParams = { 0 };
    thisParams.bufferSize = params->devices[i].bufferSize;
    thisParams.format = thisFormat;
    thisParams.stream.interleaved = params->stream.interleaved;
    thisParams.stream.onBuffer = onThisBuffer;
    thisParams.stream.onXRun = params->stream.onXRun;
    if((error = params->devices[i].device->OpenStream(&thisParams, params->master != params->devices[i].device, &result->_contexts[i], &thisStream) != 0))
      return error;
    auto thisBlocking = &dynamic_cast<XtBlockingStream&>(*thisStream);
    result->_streams.push_back(std::unique_ptr<XtBlockingStream>(thisBlocking));
    thisBlocking->_index = i;
    thisBlocking->_aggregated = true;
    if((error = XtStreamGetFrames(thisStream, &thisFrames)) != 0)
      return error;
    frames = thisFrames > frames? thisFrames: frames;
  }
  XT_ASSERT(masterFound);

  result->_params.format = format;
  result->_frames = frames * 2;
  XtiInitIOBuffers(result->_weave, &format, frames);
  XtiInitIOBuffers(result->_buffers, &format, frames);
  for(int32_t i = 0; i < params->count; i++) {
    result->_rings[i].input = XtRingBuffer(params->stream.interleaved != XtFalse, result->_frames, params->devices[i].channels.inputs, attrs.size);
    result->_rings[i].output = XtRingBuffer(params->stream.interleaved != XtFalse, result->_frames, params->devices[i].channels.outputs, attrs.size);
  }

  *stream = result.release();
  return 0;
}

// ---- device ----