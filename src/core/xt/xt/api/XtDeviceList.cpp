#include <xt/shared/Shared.hpp>
#include <xt/api/XtDeviceList.h>
#include <xt/private/DeviceList.hpp>

void XT_CALL
XtDeviceListDestroy(XtDeviceList* l)
{
  XT_ASSERT_VOID_API(XtiCalledOnMainThread());
  delete l;
}

XtError XT_CALL
XtDeviceListGetCount(XtDeviceList const* l, int32_t* count)
{
  XT_ASSERT_API(l != nullptr);
  XT_ASSERT_API(count != nullptr);
  XT_ASSERT_API(XtiCalledOnMainThread());
  *count = 0;
  return XtiCreateError(l->GetSystem(), l->GetCount(count));
}

XtError XT_CALL
XtDeviceListGetId(XtDeviceList const* l, int32_t index, char* buffer, int32_t* size)
{
  XT_ASSERT_API(index >= 0);
  XT_ASSERT_API(l != nullptr);
  XT_ASSERT_API(XtiCalledOnMainThread());
  XT_ASSERT_API(size != nullptr && *size >= 0);
  return XtiCreateError(l->GetSystem(), l->GetId(index, buffer, size));
}

XtError XT_CALL
XtDeviceListGetName(XtDeviceList const* l, char const* id, char* buffer, int32_t* size)
{
  XT_ASSERT_API(l != nullptr);
  XT_ASSERT_API(id != nullptr);
  XT_ASSERT_API(XtiCalledOnMainThread());
  XT_ASSERT_API(size != nullptr && *size >= 0);
  return XtiCreateError(l->GetSystem(), l->GetName(id, buffer, size));
}

XtError XT_CALL 
XtDeviceListGetCapabilities(XtDeviceList const* l, char const* id, XtDeviceCaps* capabilities)
{
  XT_ASSERT_API(l != nullptr);  
  XT_ASSERT_API(id != nullptr);
  XT_ASSERT_API(capabilities != nullptr);
  XT_ASSERT_API(XtiCalledOnMainThread());
  *capabilities = XtDeviceCapsNone;
  return XtiCreateError(l->GetSystem(), l->GetCapabilities(id, capabilities));
}