#include <xt/private/AggregateStream.hpp>

XtSystem
XtAggregateStream::GetSystem() const
{ return _system; }

XtFault
XtAggregateStream::GetFrames(int32_t* frames) const
{ return *frames = _frames, 0; }

XtFault
XtAggregateStream::Stop()
{
  XtFault fault;
  if(!XtiCompareExchange(_running, 1, 0)) return 0;
  while(_insideCallbackCount.load() != 0);
  XT_ASSERT(0 <= _masterIndex && _masterIndex < _streams.size());
  if((fault = _streams[_masterIndex]->Stop()) != 0) return fault;
  for(size_t i = 0; i < _streams.size(); i++)
    if(i != static_cast<size_t>(_masterIndex))
      if((fault = _streams[i]->Stop) != 0) return fault;
  return 0;
}

XtFault
XtAggregateStream::Start()
{
  XtFault fault;
  for(size_t i = 0; i < _streams.size(); i++)
  {
    _rings[i].input.Clear();
    _rings[i].output.Clear();
  }
  for(size_t i = 0; i < _streams.size(); i++)
    if(i != static_cast<size_t>(_masterIndex))
      if((fault = _streams[i]->Start()) != 0) return fault;
  if((fault = _streams[_masterIndex]->Start()) != 0)  return fault;
  XT_ASSERT(XtiCompareExchange(_running, 0, 1));
  return 0;
}

XtFault
XtAggregateStream::GetLatency(XtLatency* latency) const 
{
  XtFault fault;
  XtLatency local = { 0 };
  auto invRate = 1000.0 / _params.format.mix.rate;
  for(size_t i = 0; i < _streams.size(); i++)
  {
    if((fault = _streams[i]->GetLatency(&local)) != 0) return fault;
    if(local.input == 0.0 && local.output == 0.0) return 0;
    if(local.input > 0.0)
    {
      local.input += _rings[i].input.Full() * invRate;
      latency->input = local.input > latency->input? local.input: latency->input;
    }      
    if(local.output > 0.0)
    {
      local.output += _rings[i].output.Full() * invRate;
      latency->output = local.output > latency->output? local.output: latency->output;
    }      
  }
  return 0;
}