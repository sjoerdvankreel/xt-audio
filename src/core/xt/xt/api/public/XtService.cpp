#include <xt/api/public/XtService.h>
#include <xt/api/private/Service.hpp>
#include <xt/private/Shared.hpp>

XtCapabilities XT_CALL
XtServiceGetCapabilities(XtService const* s)
{
  XT_ASSERT(s != nullptr);
  return s->GetCapabilities();
}

XtError XT_CALL
XtServiceOpenDevice(XtService const* s, char const* id, XtDevice** device)
{
  XT_ASSERT(s != nullptr);
  XT_ASSERT(device != nullptr);
  XT_ASSERT(XtiCalledOnMainThread());
  *device = nullptr;
  return XtiCreateError(s->GetSystem(), s->OpenDevice(id, device));
}

XtError XT_CALL
XtServiceOpenDeviceList(XtService const* s, XtEnumFlags flags, XtDeviceList** list)
{ 
  XT_ASSERT(flags != 0);
  XT_ASSERT(s != nullptr);
  XT_ASSERT(list != nullptr);
  XT_ASSERT(XtiCalledOnMainThread());
  *list = nullptr;
  return XtiCreateError(s->GetSystem(), s->OpenDeviceList(flags, list));
}

XtError XT_CALL
XtServiceGetDefaultDeviceId(XtService const* s, XtBool output, XtBool* valid, char* buffer, int32_t* size)
{
  XT_ASSERT(s != nullptr);
  XT_ASSERT(valid != nullptr);
  XT_ASSERT(XtiCalledOnMainThread());
  XT_ASSERT(size != nullptr && *size >= 0);
  *valid = XtFalse;  
  return XtiCreateError(s->GetSystem(), s->GetDefaultDeviceId(output, valid, buffer, size));
}

XtError XT_CALL 
XtServiceAggregateStream(XtService const* s, XtAggregateStreamParams const* params, void* user, XtStream** stream)
{
  XT_ASSERT(s != nullptr);
  XT_ASSERT(params != nullptr);
  XT_ASSERT(params->count > 0);
  XT_ASSERT(stream != nullptr);
  XT_ASSERT(XtiCalledOnMainThread());
  XT_ASSERT(params->master != nullptr);
  XT_ASSERT(params->devices != nullptr);
  XT_ASSERT(params->stream.onBuffer != nullptr);
  XT_ASSERT((s->GetCapabilities() & XtCapabilitiesAggregation) != 0);
  return XtiCreateError(s->GetSystem(), s->AggregateStream(params, user, stream));
}