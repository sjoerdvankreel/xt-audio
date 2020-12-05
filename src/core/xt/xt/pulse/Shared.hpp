#ifndef XT_PULSE_SHARED_HPP
#define XT_PULSE_SHARED_HPP
#if XT_ENABLE_PULSE
#include <xt/api/private/Device.hpp>
#include <xt/api/private/Stream.hpp>
#include <xt/api/private/Service.hpp>
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

struct PulseStream:
public XtBlockingStream 
{
  XtPaSimple _pa;
  bool const _output;
  int32_t const _frames;
  std::vector<uint8_t> _audio;
  
  XT_IMPLEMENT_BLOCKING_STREAM(Pulse);
  PulseStream(bool secondary, XtPaSimple&& pa, bool out, int32_t frames, int32_t frameSize);
};

#endif // XT_ENABLE_PULSE
#endif // XT_PULSE_SHARED_HPP