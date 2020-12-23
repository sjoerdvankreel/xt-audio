#if XT_ENABLE_JACK
#include <xt/backend/jack/Shared.hpp>
#include <xt/backend/jack/Private.hpp>

#include <utility>

JackStream::
JackStream(XtJackClient&& jc):
_jc(std::move(jc)) { }

XtBool
JackStream::IsRunning() const
{ return _running.load() != 0; }
XtFault
JackStream::GetLatency(XtLatency* latency) const
{ return 0; }
XtFault
JackStream::GetFrames(int32_t* frames) const
{ *frames = jack_get_buffer_size(_jc.jc); return 0; }
int JackStream::XRunCallback(void* arg)
{ static_cast<JackStream*>(arg)->OnXRun(-1); return 0; }

void
JackStream::Stop()
{ 
  if(!XtiCompareExchange(_running, 1, 0)) return;
  while(_insideCallback.load() == 1);
  _connections.clear();
  XT_TRACE_IF(jack_deactivate(_jc.jc) != 0);
  OnRunning(XtFalse, 0);
}

XtFault
JackStream::Start()
{  
  XtFault fault;
  std::vector<XtJackConnection> connections;
  auto const& channels = _params.format.channels;
  
  XT_ASSERT(XtiCompareExchange(_running, 0, 1));
  auto guard = XtiGuard([this] { XtiCompareExchange(_running, 1, 0); });
  if((fault = jack_activate(_jc.jc)) != 0) return fault;

  for(int32_t i = 0; i < channels.inputs; i++)
  {
    char const* src = _inputs[i].connectTo;
    char const* dst = jack_port_name(_inputs[i].port);
    if((fault = jack_connect(_jc.jc, src, dst)) != 0) return fault;
    connections.emplace_back(XtJackConnection(_jc.jc, src, dst));
  }
  for(int32_t i = 0; i < channels.outputs; i++)
  {
    char const* dst = _outputs[i].connectTo;
    char const* src = jack_port_name(_outputs[i].port);
    if((fault = jack_connect(_jc.jc, src, dst)) != 0) return fault;
    connections.emplace_back(XtJackConnection(_jc.jc, src, dst));
  }  
  _connections = std::move(connections);
  OnRunning(XtTrue, 0);
  guard.Commit();
  return 0;
}

int 
JackStream::ProcessCallback(jack_nframes_t frames, void* arg)
{    
  XtBuffer buffer = { 0 };
  buffer.frames = frames;
  JackStream* s = static_cast<JackStream*>(arg);
  buffer.input = s->_inputs.empty()? nullptr: s->_inputChannels.data();
  buffer.output = s->_outputs.empty()? nullptr: s->_outputChannels.data(); 

  if(s->_running.load() != 1) return 0;
  if(!XtiCompareExchange(s->_insideCallback, 0, 1)) return 0;

  float period;
  jack_time_t time, next;
  jack_nframes_t position;
  if(jack_get_cycle_times(s->_jc.jc, &position, &time, &next, &period) == 0)
  {
    buffer.timeValid = XtTrue;
    buffer.position = position;
    buffer.time = time / 1000.0;
  }

  for(int32_t i = 0; i < s->_inputs.size(); i++)
    s->_inputChannels[i] = jack_port_get_buffer(s->_inputs[i].port, frames);
  for(int32_t i = 0; i < s->_outputs.size(); i++)
    s->_outputChannels[i] = jack_port_get_buffer(s->_outputs[i].port, frames);
  XT_ASSERT(s->OnBuffer(-1, &buffer) == 0);  
  XT_ASSERT(XtiCompareExchange(s->_insideCallback, 1, 0));
  return 0;
}

#endif // XT_ENABLE_JACK