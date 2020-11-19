#include <xt/audio/XtDevice.h>
#include <xt/Private.hpp>
#include <cstring>

void XT_CALL
XtDeviceDestroy(XtDevice* d) 
{
  XT_ASSERT(XtiCalledOnMainThread());
  delete d;
}

XtError XT_CALL
XtDeviceShowControlPanel(XtDevice* d) 
{
  XT_ASSERT(d != nullptr);
  XT_ASSERT(XtiCalledOnMainThread());
  return XtiCreateError(d->GetSystem(), d->ShowControlPanel());
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
XtDeviceGetName(XtDevice const* d, char* buffer, int32_t* size)
{
  XT_ASSERT(d != nullptr);
  XT_ASSERT(XtiCalledOnMainThread());
  XT_ASSERT(size != nullptr && *size >= 0);
  return XtiCreateError(d->GetSystem(), d->GetName(buffer, size));
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
  XT_ASSERT(XtiValidateFormat(d->GetSystem(), *format));
  *supports = XtFalse;
  return XtiCreateError(d->GetSystem(), d->SupportsFormat(format, supports));
}

XtError XT_CALL
XtDeviceGetBufferSize(XtDevice const* d, XtFormat const* format, XtBufferSize* size)
{
  XT_ASSERT(d != nullptr);
  XT_ASSERT(size != nullptr);
  XT_ASSERT(format != nullptr);
  XT_ASSERT(XtiCalledOnMainThread());
  XT_ASSERT(XtiValidateFormat(d->GetSystem(), *format));
  XtError error;
  XtBool supports;
  XtSystem system = d->GetSystem();
  std::memset(size, 0, sizeof(XtBufferSize));
  if((error = XtDeviceSupportsFormat(d, format, &supports)) != 0) return error;
  if(!supports) return XtiCreateError(system, XtAudioGetService(system)->GetFormatFault());
  return XtiCreateError(d->GetSystem(), d->GetBufferSize(format, size));
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