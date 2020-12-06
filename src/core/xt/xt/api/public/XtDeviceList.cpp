#include <xt/api/public/XtDeviceList.h>
#include <xt/api/private/DeviceList.hpp>
#include <xt/private/Shared.hpp>

void XT_CALL
XtDeviceListDestroy(XtDeviceList* l)
{
  XT_ASSERT(XtiCalledOnMainThread());
  delete l;
}

XtError XT_CALL
XtDeviceListGetCount(XtDeviceList const* l, int32_t* count)
{
  XT_ASSERT(l != nullptr);
  XT_ASSERT(count != nullptr);
  XT_ASSERT(XtiCalledOnMainThread());
  *count = 0;
  return XtiCreateError(l->GetSystem(), l->GetCount(count));
}

XtError XT_CALL
XtDeviceListGetId(XtDeviceList const* l, int32_t index, char* buffer, int32_t* size)
{
  XT_ASSERT(index >= 0);
  XT_ASSERT(l != nullptr);
  XT_ASSERT(XtiCalledOnMainThread());
  XT_ASSERT(size != nullptr && *size >= 0);
  return XtiCreateError(l->GetSystem(), l->GetId(index, buffer, size));
}

XtError XT_CALL
XtDeviceListGetName(XtDeviceList const* l, char const* id, char* buffer, int32_t* size)
{
  XT_ASSERT(l != nullptr);
  XT_ASSERT(id != nullptr);
  XT_ASSERT(XtiCalledOnMainThread());
  XT_ASSERT(size != nullptr && *size >= 0);
  return XtiCreateError(l->GetSystem(), l->GetName(id, buffer, size));
}

XtError XT_CALL
XtDeviceListGetDefaultId(XtDeviceList const* l, XtBool output, XtBool* valid, char* buffer, int32_t* size)
{
  XT_ASSERT(l != nullptr);
  XT_ASSERT(valid != nullptr);
  XT_ASSERT(XtiCalledOnMainThread());
  XT_ASSERT(size != nullptr && *size >= 0);
  *valid = XtFalse;  
  return XtiCreateError(l->GetSystem(), l->GetDefaultId(output, valid, buffer, size));
}