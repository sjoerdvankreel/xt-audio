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
  XT_IMPLEMENT_SERVICE();
};

struct PulseDevice: 
public XtDevice
{
  const bool _output;
  XT_IMPLEMENT_DEVICE();
  PulseDevice(bool output);
};

struct PulseStream:
public XtBlockingStream 
{
  bool const _output;
  XtPaSimple _client;
  std::vector<uint8_t> _audio;
  int32_t const _bufferFrames;
  
  XT_IMPLEMENT_BLOCKING_STREAM();
  PulseStream(bool secondary, XtPaSimple&& c, bool output, int32_t bufferFrames, int32_t frameSize);
};

#endif // XT_ENABLE_PULSE
#endif // XT_PULSE_SHARED_HPP