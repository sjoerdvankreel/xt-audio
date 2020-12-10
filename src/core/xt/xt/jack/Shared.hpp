#ifndef XT_JACK_SHARED_HPP
#define XT_JACK_SHARED_HPP
#if XT_ENABLE_JACK
#include <xt/jack/Private.hpp>
#include <jack/jack.h>
#include <vector>

struct JackDevice:
public XtDevice
{
  XtJackClient _jc;
  XT_IMPLEMENT_DEVICE(JACK);
};

struct JackService:
public XtService 
{
  JackService();
  ~JackService();
  XT_IMPLEMENT_SERVICE(JACK);
};

struct JackDeviceList:
public XtDeviceList
{
  XT_IMPLEMENT_DEVICE_LIST(JACK);
};

struct JackStream:
public XtStream
{
  XtJackClient _jc;
  std::vector<XtJackPort> _inputs;
  std::vector<XtJackPort> _outputs;
  std::vector<void*> _inputChannels;
  std::vector<void*> _outputChannels;
  std::vector<XtJackConnection> _connections;

  JackStream() = default;
  XT_IMPLEMENT_STREAM();
  XT_IMLEMENT_STREAM_SYSTEM(JACK);

  static int XRunCallback(void* arg);
  static int ProcessCallback(jack_nframes_t frames, void* arg);
};

#endif // XT_ENABLE_JACK
#endif // XT_JACK_SHARED_HPP