#ifndef XT_PULSE_SHARED_HPP
#define XT_PULSE_SHARED_HPP
#if XT_ENABLE_PULSE
#include <xt/private/Device.hpp>
#include <xt/private/Stream.hpp>
#include <xt/private/Service.hpp>
#include <xt/blocking/Device.hpp>
#include <xt/blocking/Stream.hpp>
#include <xt/private/DeviceList.hpp>
#include <xt/backend/pulse/Private.hpp>

#include <pulse/simple.h>
#include <pulse/pulseaudio.h>
#include <cstdint>
#include <vector>

struct PulseService final: 
public XtService 
{
  XT_IMPLEMENT_SERVICE(Pulse);
};

struct PulseDevice final: 
public XtBlockingDevice
{
  bool const _output;
  XT_IMPLEMENT_DEVICE();
  XT_IMPLEMENT_DEVICE_BLOCKING();
  XT_IMPLEMENT_DEVICE_BASE(Pulse);
  PulseDevice(bool output);
};

struct PulseStream final:
public XtBlockingStream 
{
  bool _output;
  XtPaSimple _pa;
  int32_t _frames;
  std::vector<uint8_t> _audio;
  
  PulseStream() = default;
  XT_IMPLEMENT_STREAM_BASE();
  XT_IMPLEMENT_BLOCKING_STREAM();
  XT_IMPLEMENT_STREAM_BASE_SYSTEM(Pulse);
};

struct PulseDeviceList final:
public XtDeviceList 
{
  bool const _input;
  bool const _output;
  XT_IMPLEMENT_DEVICE_LIST(Pulse);
  PulseDeviceList(bool input, bool output);
};

#endif // XT_ENABLE_PULSE
#endif // XT_PULSE_SHARED_HPP