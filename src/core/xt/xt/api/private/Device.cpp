#include <xt/api/public/XtStream.h>
#include <xt/api/private/Device.hpp>
#include <xt/api/private/Stream.hpp>
#include <memory>

XtFault
XtDevice::OpenStream(XtDeviceStreamParams const* params, bool secondary, void* user, XtStream** stream)
{
  XtFault fault;
  int32_t frames;
  XtBool supports;

  *stream = nullptr;
  std::unique_ptr<XtStream> ptr;
  if((fault = SupportsAccess(params->stream.interleaved, &supports)) != 0) return fault;
  if((fault = OpenStreamCore(params, secondary, user, stream)) != 0) return fault;
  ptr.reset(*stream);
  if((fault = ptr->GetFrames(&frames)) != 0) return fault;

  (*stream)->_user = user;
  (*stream)->_params = *params;
  (*stream)->_emulated = !supports;
  XtiInitIOBuffers((*stream)->_buffers, &params->format, frames);
  ptr.release();
  return 0;
}