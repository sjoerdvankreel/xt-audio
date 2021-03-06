#include <xt/api/XtStream.h>
#include <xt/private/Device.hpp>
#include <xt/private/Stream.hpp>
#include <memory>

XtFault
XtDevice::OpenStream(XtDeviceStreamParams const* params, void* user, XtStream** stream)
{
  XtFault fault;
  int32_t frames;
  XtBool supports;

  *stream = nullptr;
  std::unique_ptr<XtStream> ptr;
  if((fault = SupportsAccess(params->stream.interleaved, &supports)) != 0) return fault;
  if((fault = OpenStreamCore(params, stream)) != 0) return fault;
  ptr.reset(*stream);
  if((fault = ptr->GetFrames(&frames)) != 0) return fault;

  (*stream)->_user = user;
  (*stream)->_params = *params;
  (*stream)->_emulated = !supports;
  XtiInitIOBuffers((*stream)->_buffers, &params->format, frames);
  ptr.release();
  return 0;
}