#include <xt/blocking/Device.hpp>
#include <xt/blocking/Adapter.hpp>

XtFault 
XtBlockingDevice::OpenStreamCore(XtDeviceStreamParams const* params, XtStream** stream)
{
  XtFault fault;
  XtBlockingParams blockingParams = { 0 };
  XtBlockingStream* blockingStream = nullptr;
  blockingParams.index = -1;
  blockingParams.format = params->format;
  blockingParams.bufferSize = params->bufferSize;
  blockingParams.interleaved = params->stream.interleaved;
  if((fault = OpenBlockingStream(&blockingParams, &blockingStream)) != 0) return fault;  
  blockingStream->_params = blockingParams;
  *stream = new XtBlockingAdapter(blockingStream);
  return 0;
}