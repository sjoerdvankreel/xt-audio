#include <xt/api/public/XtService.h>
#include <xt/Private.hpp>

XtCapabilities XT_CALL
XtServiceGetCapabilities(XtService const* s)
{
  XT_ASSERT(s != nullptr);
  return s->GetCapabilities();
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
XtServiceOpenDefaultDevice(XtService const* s, XtBool output, XtDevice** device)
{
  XT_ASSERT(s != nullptr);
  XT_ASSERT(device != nullptr);
  XT_ASSERT(XtiCalledOnMainThread());
  *device = nullptr;
  return XtiCreateError(s->GetSystem(), s->OpenDefaultDevice(output, device));
}