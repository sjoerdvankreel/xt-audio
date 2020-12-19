#include <xt/private/BlockingDevice.hpp>
#include <xt/private/BlockingAdapter.hpp>

XtFault 
XtBlockingDevice::OpenStreamCore(XtDeviceStreamParams const* params, XtStream** stream)
{
  XtFault fault;
  XtBlockingStream* blocking = nullptr;
  if((fault = OpenBlockingStream(params, &blocking)) != 0) return fault;  
  *stream = new XtBlockingAdapter(blocking);
  return 0;
}