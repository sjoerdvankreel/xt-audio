#ifndef XT_JACK_SHARED_HPP
#define XT_JACK_SHARED_HPP
#if XT_ENABLE_JACK
#include <xt/jack/Private.hpp>
#include <jack/jack.h>

struct JackService:
public XtService 
{
  JackService();
  ~JackService();
  XT_IMPLEMENT_SERVICE(JACK);
};

struct JackDevice:
public XtDevice
{
  XtJackClient _jc;
  XT_IMPLEMENT_DEVICE(JACK);
};

struct JackDeviceList:
public XtDeviceList
{
  XT_IMPLEMENT_DEVICE_LIST(JACK);
};

#endif // XT_ENABLE_JACK
#endif // XT_JACK_SHARED_HPP