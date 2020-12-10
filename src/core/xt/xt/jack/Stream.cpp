#if XT_ENABLE_JACK
#include <xt/jack/Shared.hpp>
#include <xt/jack/Private.hpp>

XtFault
JackStream::GetLatency(XtLatency* latency) const
{ return 0; }
XtFault
JackStream::GetFrames(int32_t* frames) const
{ *frames = jack_get_buffer_size(_jc.jc); return 0; }
int JackStream::XRunCallback(void* arg)
{ static_cast<JackStream*>(arg)->OnXRun(); return 0; }

XtFault
JackStream::Stop()
{
  _connections.clear();
  return jack_deactivate(_jc.jc);
}

XtFault
JackStream::Start()
{  
  XtFault fault;
  std::vector<XtJackConnection> connections;
  auto const& channels = _params.format.channels;
  if((fault = jack_activate(_jc.jc)) != 0) return fault;

  for(int32_t i = 0; i < channels.inputs; i++)
  {
    char const* src = inputs[i].connectTo;
    char const* dst = jack_port_name(inputs[i].port);
    if((fault = jack_connect(_jc.jc, src, dst)) != 0) return fault;
    connections.emplace_back(XtJackConnection(_jc.jc, src, dst));
  }
  for(int32_t i = 0; i < _channels.outputs; i++)
  {
    char const* dst = outputs[i].connectTo;
    char const* src = jack_port_name(outputs[i].port);
    if((fault = jack_connect(_jc.jc, src, dst)) != 0) return fault;
    connections.emplace_back(XtJackConnection(_jc.jc, src, dst));
  }  
  _connections = std::move(connections);
  return 0;
}

int 
JackStream::ProcessCallback(jack_nframes_t frames, void* arg)
{    
  XtBuffer buffer = { 0 };
  buffer.frames = frames;
  JackStream* s = static_cast<JackStream*>(arg);
  buffer.input = s->_inputs.empty()? nullptr: s->inputChannels.data();
  buffer.output = s->_outputs.empty()? nullptr: s->outputChannels.data(); 

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
  s->OnBuffer(&buffer);
  return 0;
}

#endif // XT_ENABLE_JACK