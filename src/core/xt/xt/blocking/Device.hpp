#ifndef XT_BLOCKING_DEVICE_HPP
#define XT_BLOCKING_DEVICE_HPP

#include <xt/shared/Structs.hpp>
#include <xt/private/Device.hpp>
#include <xt/blocking/Stream.hpp>

#define XT_IMPLEMENT_DEVICE_BLOCKING() \
  XtFault OpenBlockingStream(XtBlockingParams const* params, XtBlockingStream** stream) override final;

struct XtBlockingDevice:
public XtDevice
{ 
  XT_IMPLEMENT_DEVICE_STREAM();
  virtual XtFault OpenBlockingStream(XtBlockingParams const* params, XtBlockingStream** stream) = 0;
};

#endif // XT_BLOCKING_DEVICE_HPP