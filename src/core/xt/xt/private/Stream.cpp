#include <xt/shared/Shared.hpp>
#include <xt/private/Stream.hpp>

void
XtStream::OnXRun(int32_t index) const
{
  auto onXRun = _params.stream.onXRun;
  if(onXRun != nullptr) onXRun(this, index, _user);
}

void
XtStream::OnRunning(XtBool running) const
{
  auto onRunning = _params.stream.onRunning;
  if(onRunning != nullptr) onRunning(this, running, _user);
}

XtFault
XtStream::OnBuffer(int32_t index, XtBuffer const* buffer)
{
  XtOnBufferParams params = { 0 };
  params.index = index;
  params.buffer = buffer;
  params.buffers = &_buffers;
  params.emulated = _emulated;
  params.format = &_params.format;
  params.interleaved = _params.stream.interleaved;
  return XtiOnBuffer(&params, [this](XtBuffer const* converted) { 
    return _params.stream.onBuffer(this, converted, _user); });
}