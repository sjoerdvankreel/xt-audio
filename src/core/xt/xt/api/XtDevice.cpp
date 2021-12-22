#include <xt/api/XtDevice.h>
#include <xt/shared/Shared.hpp>
#include <xt/private/Device.hpp>
#include <xt/private/Platform.hpp>

#include <cstring>

void XT_CALL
XtDeviceDestroy(XtDevice* d) 
{
  XT_ASSERT_VOID_API(XtiCalledOnMainThread());
  delete d;
}

void* XT_CALL
XtDeviceGetHandle(XtDevice const* d)
{
  XT_ASSERT_API(d != nullptr);
  return d->GetHandle();
}

XtError XT_CALL
XtDeviceGetMix(XtDevice const* d, XtBool* valid, XtMix* mix)
{
  XT_ASSERT_API(d != nullptr);
  XT_ASSERT_API(mix != nullptr);
  XT_ASSERT_API(valid != nullptr);
  XT_ASSERT_API(XtiCalledOnMainThread());
  *valid = XtFalse;
  std::memset(mix, 0, sizeof(XtMix));
  return XtiCreateError(d->GetSystem(), d->GetMix(valid, mix));
}

XtError XT_CALL
XtDeviceGetChannelCount(XtDevice const* d, XtBool output, int32_t* count)
{
  XT_ASSERT_API(d != nullptr);
  XT_ASSERT_API(count != nullptr);
  XT_ASSERT_API(XtiCalledOnMainThread());
  *count = 0;
  return XtiCreateError(d->GetSystem(), d->GetChannelCount(output, count));
}

XtError XT_CALL
XtDeviceSupportsAccess(XtDevice const* d, XtBool interleaved, XtBool* supports)
{
  XT_ASSERT_API(d != nullptr);
  XT_ASSERT_API(supports != nullptr);
  XT_ASSERT_API(XtiCalledOnMainThread());
  *supports = XtFalse;
  return XtiCreateError(d->GetSystem(), d->SupportsAccess(interleaved, supports));
}

XtError XT_CALL
XtDeviceSupportsFormat(XtDevice const* d, XtFormat const* format, XtBool* supports)
{
  XT_ASSERT_API(d != nullptr);
  XT_ASSERT_API(format != nullptr);
  XT_ASSERT_API(supports != nullptr);
  XT_ASSERT_API(XtiCalledOnMainThread());
  *supports = XtFalse;
  return XtiCreateError(d->GetSystem(), d->SupportsFormat(format, supports));
}

XtError XT_CALL
XtDeviceGetBufferSize(XtDevice const* d, XtFormat const* format, XtBufferSize* size)
{
  XtFault fault;
  XT_ASSERT_API(d != nullptr);
  XT_ASSERT_API(size != nullptr);
  XT_ASSERT_API(format != nullptr);
  XT_ASSERT_API(XtiCalledOnMainThread());
  std::memset(size, 0, sizeof(XtBufferSize));  
  if((fault = XtiSupportsFormat(d, format)) != 0) return XtiCreateError(d->GetSystem(), fault);
  return XtiCreateError(d->GetSystem(), d->GetBufferSize(format, size));
}

XtError XT_CALL
XtDeviceGetChannelName(XtDevice const* d, XtBool output, int32_t index, char* buffer, int32_t* size)
{
  XT_ASSERT_API(index >= 0);
  XT_ASSERT_API(d != nullptr);
  XT_ASSERT_API(XtiCalledOnMainThread());  
  XT_ASSERT_API(size != nullptr && *size >= 0);
  return XtiCreateError(d->GetSystem(), d->GetChannelName(output, index, buffer, size));
}

XtError XT_CALL 
XtDeviceOpenStream(XtDevice* d, XtDeviceStreamParams const* params, void* user, XtStream** stream)
{  
  XtFault fault;
  XT_ASSERT_API(d != nullptr);
  XT_ASSERT_API(params != nullptr);
  XT_ASSERT_API(stream != nullptr);
  XT_ASSERT_API(XtiCalledOnMainThread());
  XT_ASSERT_API(params->bufferSize > 0.0);
  XT_ASSERT_API(params->stream.onBuffer != nullptr);
  if((fault = XtiSupportsFormat(d, &params->format)) != 0) return XtiCreateError(d->GetSystem(), fault);
  return XtiCreateError(d->GetSystem(), d->OpenStream(params, user, stream));
}

XtError XT_CALL
XtDeviceShowControlPanel(XtDevice* d) 
{
  XT_ASSERT_API(d != nullptr);
  XT_ASSERT_API(XtiCalledOnMainThread());
  XT_ASSERT_API((XtPlatform::instance->GetService(d->GetSystem())->GetCapabilities() & XtServiceCapsControlPanel) != 0);
  return XtiCreateError(d->GetSystem(), d->ShowControlPanel());
}