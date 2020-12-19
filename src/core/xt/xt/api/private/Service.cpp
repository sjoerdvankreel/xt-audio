#include <xt/api/public/Shared.h>
#include <xt/api/public/XtAudio.h>
#include <xt/api/public/Structs.h>
#include <xt/api/private/Stream.hpp>
#include <xt/api/private/Device.hpp>
#include <xt/api/private/Service.hpp>
#include <xt/private/AggregateStream.hpp>
#include <memory>
#include <cstring>

XtFault
XtService::AggregateStream(XtAggregateStreamParams const* params, void* user, XtStream** stream) const
{
  return 0;
#if 0
  XtFault fault;
  bool masterFound = false;
  bool interleaved = params->stream.interleaved != XtFalse;
  auto attrs = XtAudioGetSampleAttributes(params->mix.sample);  
  std::unique_ptr<XtAggregateStream> result(new XtAggregateStream); 

  XtFormat format = { 0 };
  format.mix = params->mix;
  result->_frames = 0;
  result->_masterIndex = -1;

  for(int32_t i = 0; i < params->count; i++)
  {
    auto const& device = params->devices[i];
    result->_channels.push_back(device.channels);

    XtFormat thisFormat = { 0 };
    thisFormat.mix = params->mix;
    thisFormat.channels = device.channels;
    bool isMaster = params->master == device.device;
    format.channels.inputs += device.channels.inputs;
    format.channels.outputs += device.channels.outputs;
    masterFound |= isMaster;

    XtOnBuffer onThisBuffer = XtAggregateStream::OnSlaveBuffer;
    if(isMaster)
    {
      result->_masterIndex = i;
      onThisBuffer = XtAggregateStream::OnMasterBuffer;
    }

    XtStream* thisStream;
    XtDeviceStreamParams thisParams = { 0 };
    thisParams.format = thisFormat;
    thisParams.bufferSize = device.bufferSize;
    thisParams.stream.onBuffer = onThisBuffer;
    thisParams.stream.onXRun = params->stream.onXRun;
    thisParams.stream.interleaved = params->stream.interleaved;
    if((fault = device.device->OpenStream(&thisParams, !isMaster, &result->_contexts[i], &thisStream) != 0)) return fault;

    int32_t thisFrames;
    auto thisBlocking = &dynamic_cast<XtBlockingStream&>(*thisStream);
    result->_streams.push_back(std::unique_ptr<XtBlockingStream>(thisBlocking));
    thisBlocking->_index = i;
    thisBlocking->_aggregated = true;    
    if((fault = thisStream->GetFrames(&thisFrames)) != 0) return fault;
    result->_frames = thisFrames > result->_frames? thisFrames: result->_frames;
  }

  result->_frames *= 2;
  XT_ASSERT(masterFound);
  XtiInitIOBuffers(result->_weave, &result->_params.format, result->_frames);
  XtiInitIOBuffers(result->_buffers, &result->_params.format, result->_frames);
  for(int32_t i = 0; i < params->count; i++)
  {
    XtIORingBuffers thisRings;
    thisRings.input = XtRingBuffer(interleaved, result->_frames, params->devices[i].channels.inputs, attrs.size);
    thisRings.output = XtRingBuffer(interleaved, result->_frames, params->devices[i].channels.outputs, attrs.size);
    result->_rings.push_back(thisRings);
  }

  *stream = result.release();
  return 0;
#endif
}