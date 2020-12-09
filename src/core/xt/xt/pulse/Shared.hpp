#ifndef XT_PULSE_SHARED_HPP
#define XT_PULSE_SHARED_HPP
#if XT_ENABLE_PULSE
#include <xt/api/private/Device.hpp>
#include <xt/api/private/Stream.hpp>
#include <xt/api/private/Service.hpp>
#include <xt/api/private/DeviceList.hpp>
#include <xt/private/BlockingStream.hpp>
#include <xt/pulse/Private.hpp>
#include <pulse/simple.h>
#include <pulse/pulseaudio.h>
#include <cstdint>
#include <vector>

struct PulseService: 
public XtService 
{
  XT_IMPLEMENT_SERVICE(Pulse);
};

struct PulseDevice: 
public XtDevice
{
  const bool _output;
  XT_IMPLEMENT_DEVICE(Pulse);
  PulseDevice(bool output);
};

struct PulseDeviceList:
public XtDeviceList 
{
  XT_IMPLEMENT_DEVICE_LIST(Pulse);
};

struct PulseStream:
public XtBlockingStream 
{
  bool _output;
  XtPaSimple _pa;
  int32_t _frames;
  std::vector<uint8_t> _audio;
  
  PulseStream(bool secondary);
  XT_IMPLEMENT_BLOCKING_STREAM(Pulse);
};

#endif // XT_ENABLE_PULSE
#endif // XT_PULSE_SHARED_HPP