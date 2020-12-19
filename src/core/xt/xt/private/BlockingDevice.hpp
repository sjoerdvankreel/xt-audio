#ifndef XT_PRIVATE_BLOCKING_DEVICE_HPP
#define XT_PRIVATE_BLOCKING_DEVICE_HPP

#include <xt/private/Structs.hpp>
#include <xt/api/private/Device.hpp>
#include <xt/private/BlockingStream.hpp>

#define XT_IMPLEMENT_DEVICE_BLOCKING() \
  XtFault OpenBlockingStream(XtBlockingParams const* params, XtBlockingStream** stream) override final;

struct XtBlockingDevice:
public XtDevice
{ 
  XT_IMPLEMENT_DEVICE_STREAM();
  virtual XtFault OpenBlockingStream(XtBlockingParams const* params, XtBlockingStream** stream) = 0;
};

#endif // XT_PRIVATE_BLOCKING_DEVICE_HPP