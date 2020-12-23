#include <xt/api/XtDevice.h>
#include <xt/shared/Shared.hpp>
#include <xt/private/Device.hpp>
#include <xt/private/Platform.hpp>

#include <cstring>

void* XT_CALL
XtDeviceGetHandle(XtDevice* d)
{
  XT_ASSERT(d != nullptr);
  return d->GetHandle();
}

void XT_CALL
XtDeviceDestroy(XtDevice* d) 
{
  XT_ASSERT(XtiCalledOnMainThread());
  delete d;
}

XtError XT_CALL
XtDeviceGetMix(XtDevice const* d, XtBool* valid, XtMix* mix)
{
  XT_ASSERT(d != nullptr);
  XT_ASSERT(mix != nullptr);
  XT_ASSERT(XtiCalledOnMainThread());
  *valid = XtFalse;
  std::memset(mix, 0, sizeof(XtMix));
  return XtiCreateError(d->GetSystem(), d->GetMix(valid, mix));
}

XtError XT_CALL
XtDeviceGetChannelCount(XtDevice const* d, XtBool output, int32_t* count)
{
  XT_ASSERT(d != nullptr);
  XT_ASSERT(count != nullptr);
  XT_ASSERT(XtiCalledOnMainThread());
  *count = 0;
  return XtiCreateError(d->GetSystem(), d->GetChannelCount(output, count));
}

XtError XT_CALL
XtDeviceSupportsAccess(XtDevice const* d, XtBool interleaved, XtBool* supports)
{
  XT_ASSERT(d != nullptr);
  XT_ASSERT(supports != nullptr);
  XT_ASSERT(XtiCalledOnMainThread());
  *supports = XtFalse;
  return XtiCreateError(d->GetSystem(), d->SupportsAccess(interleaved, supports));
}

XtError XT_CALL
XtDeviceSupportsFormat(XtDevice const* d, XtFormat const* format, XtBool* supports)
{
  XT_ASSERT(d != nullptr);
  XT_ASSERT(format != nullptr);
  XT_ASSERT(supports != nullptr);
  XT_ASSERT(XtiCalledOnMainThread());
  *supports = XtFalse;
  return XtiCreateError(d->GetSystem(), d->SupportsFormat(format, supports));
}

XtError XT_CALL
XtDeviceGetBufferSize(XtDevice const* d, XtFormat const* format, XtBufferSize* size)
{
  XtFault fault;
  XT_ASSERT(d != nullptr);
  XT_ASSERT(size != nullptr);
  XT_ASSERT(format != nullptr);
  XT_ASSERT(XtiCalledOnMainThread());
  std::memset(size, 0, sizeof(XtBufferSize));  
  if((fault = XtiSupportsFormat(d, format)) != 0) return XtiCreateError(d->GetSystem(), fault);
  return XtiCreateError(d->GetSystem(), d->GetBufferSize(format, size));
}

XtError XT_CALL 
XtDeviceOpenStream(XtDevice* d, const XtDeviceStreamParams* params, void* user, XtStream** stream)
{  
  XtFault fault;
  XT_ASSERT(d != nullptr);
  XT_ASSERT(params != nullptr);
  XT_ASSERT(stream != nullptr);
  XT_ASSERT(XtiCalledOnMainThread());
  XT_ASSERT(params->bufferSize > 0.0);
  XT_ASSERT(params->stream.onBuffer != nullptr);
  if((fault = XtiSupportsFormat(d, &params->format)) != 0) return XtiCreateError(d->GetSystem(), fault);
  return d->OpenStream(params, user, stream);
}

XtError XT_CALL
XtDeviceGetChannelName(XtDevice const* d, XtBool output, int32_t index, char* buffer, int32_t* size)
{
  XT_ASSERT(index >= 0);
  XT_ASSERT(d != nullptr);
  XT_ASSERT(XtiCalledOnMainThread());  
  XT_ASSERT(size != nullptr && *size >= 0);
  return XtiCreateError(d->GetSystem(), d->GetChannelName(output, index, buffer, size));
}

XtError XT_CALL
XtDeviceShowControlPanel(XtDevice* d) 
{
  XT_ASSERT(d != nullptr);
  XT_ASSERT(XtiCalledOnMainThread());
  XT_ASSERT((XtPlatform::instance->GetService(d->GetSystem())->GetCapabilities() & XtCapabilitiesControlPanel) != 0);
  return XtiCreateError(d->GetSystem(), d->ShowControlPanel());
}