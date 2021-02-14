#include <xt/api/XtService.h>
#include <xt/shared/Shared.hpp>
#include <xt/private/Service.hpp>

XtServiceCaps XT_CALL
XtServiceGetCapabilities(XtService const* s)
{
  XT_ASSERT_API(s != nullptr);
  return s->GetCapabilities();
}

XtError XT_CALL
XtServiceOpenDevice(XtService const* s, char const* id, XtDevice** device)
{
  XT_ASSERT_API(s != nullptr);
  XT_ASSERT_API(device != nullptr);
  XT_ASSERT_API(XtiCalledOnMainThread());
  *device = nullptr;
  return XtiCreateError(s->GetSystem(), s->OpenDevice(id, device));
}

XtError XT_CALL
XtServiceOpenDeviceList(XtService const* s, XtEnumFlags flags, XtDeviceList** list)
{ 
  XT_ASSERT_API(flags != 0);
  XT_ASSERT_API(s != nullptr);
  XT_ASSERT_API(list != nullptr);
  XT_ASSERT_API(XtiCalledOnMainThread());
  *list = nullptr;
  return XtiCreateError(s->GetSystem(), s->OpenDeviceList(flags, list));
}

XtError XT_CALL
XtServiceGetDefaultDeviceId(XtService const* s, XtBool output, XtBool* valid, char* buffer, int32_t* size)
{
  XT_ASSERT_API(s != nullptr);
  XT_ASSERT_API(valid != nullptr);
  XT_ASSERT_API(XtiCalledOnMainThread());
  XT_ASSERT_API(size != nullptr && *size >= 0);
  *valid = XtFalse;  
  return XtiCreateError(s->GetSystem(), s->GetDefaultDeviceId(output, valid, buffer, size));
}

XtError XT_CALL 
XtServiceAggregateStream(XtService const* s, XtAggregateStreamParams const* params, void* user, XtStream** stream)
{
  XT_ASSERT_API(s != nullptr);
  XT_ASSERT_API(params != nullptr);
  XT_ASSERT_API(params->count > 0);
  XT_ASSERT_API(stream != nullptr);
  XT_ASSERT_API(XtiCalledOnMainThread());
  XT_ASSERT_API(params->master != nullptr);
  XT_ASSERT_API(params->devices != nullptr);
  XT_ASSERT_API(params->stream.onBuffer != nullptr);
  XT_ASSERT_API((s->GetCapabilities() & XtServiceCapsAggregation) != 0);
  return XtiCreateError(s->GetSystem(), s->AggregateStream(params, user, stream));
}