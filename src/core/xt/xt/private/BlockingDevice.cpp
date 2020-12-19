#include <xt/private/BlockingDevice.hpp>
#include <xt/private/BlockingAdapter.hpp>

XtFault 
XtBlockingDevice::OpenStreamCore(XtDeviceStreamParams const* params, XtStream** stream)
{
  XtFault fault;
  XtBlockingParams blockingParams = { 0 };
  XtBlockingStream* blockingStream = nullptr;
  blockingParams.format = params->format;
  blockingParams.bufferSize = params->bufferSize;
  blockingParams.interleaved = params->stream.interleaved;
  if((fault = OpenBlockingStream(&blockingParams, &blockingStream)) != 0) return fault;  
  *stream = new XtBlockingAdapter(blockingStream);
  return 0;
}