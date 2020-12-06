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
XtServiceOpenDeviceList(XtService const* s, XtDeviceList** list)
{ 
  XT_ASSERT(s != nullptr);
  XT_ASSERT(list != nullptr);
  XT_ASSERT(XtiCalledOnMainThread());
  *list = nullptr;
  return XtiCreateError(s->GetSystem(), s->OpenDeviceList(list));
}

XtError XT_CALL
XtServiceGetDeviceCount(XtService const* s, int32_t* count)
{
  XT_ASSERT(s != nullptr);
  XT_ASSERT(count != nullptr);
  XT_ASSERT(XtiCalledOnMainThread());
  *count = 0;
  return XtiCreateError(s->GetSystem(), s->GetDeviceCount(count));
}

XtError XT_CALL
XtServiceOpenDevice(XtService const* s, int32_t index, XtDevice** device)
{
  XT_ASSERT(index >= 0);
  XT_ASSERT(s != nullptr);
  XT_ASSERT(device != nullptr);
  XT_ASSERT(XtiCalledOnMainThread());
  *device = nullptr;
  return XtiCreateError(s->GetSystem(), s->OpenDevice(index, device));
}

XtError XT_CALL
XtServiceOpenDevice2(XtService const* s, char const* id, XtDevice** device)
{
  XT_ASSERT(s != nullptr);
  XT_ASSERT(device != nullptr);
  XT_ASSERT(XtiCalledOnMainThread());
  *device = nullptr;
  return XtiCreateError(s->GetSystem(), s->OpenDevice2(id, device));
}

XtError XT_CALL
XtServiceOpenDefaultDevice(XtService const* s, XtBool output, XtDevice** device)
{
  XT_ASSERT(s != nullptr);
  XT_ASSERT(device != nullptr);
  XT_ASSERT(XtiCalledOnMainThread());
  *device = nullptr;
  return XtiCreateError(s->GetSystem(), s->OpenDefaultDevice(output, device));
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