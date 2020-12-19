#if 0
#include <xt/private/AggregateStream.hpp>
#include <xt/api/private/Stream.hpp>
#include <xt/private/Shared.hpp>

XtSystem
XtAggregateStream::GetSystem() const
{ return _streams[_masterIndex]->GetSystem(); }
XtFault
XtAggregateStream::GetFrames(int32_t* frames) const
{ return *frames = _frames, 0; }
void
XtAggregateStream::StopMasterBuffer()
{ _streams[_masterIndex]->StopMasterBuffer(); }
XtFault
XtAggregateStream::StartMasterBuffer()
{ return _streams[_masterIndex]->StartMasterBuffer(); }
XtFault
XtAggregateStream::BlockMasterBuffer()
{ return _streams[_masterIndex]->StartMasterBuffer(); }

XtFault
XtAggregateStream::PrefillOutputBuffer()
{
  for(size_t i = 0; i < _streams.size(); i++)
      _streams[i]->PrefillOutputBuffer();
}

void
XtAggregateStream::StopSlaveBuffer()
{
  _streams[_masterIndex]->StopSlaveBuffer();
  for(size_t i = 0; i < _streams.size(); i++)
    if(i != static_cast<size_t>(_masterIndex))
      _streams[i]->StopSlaveBuffer();
}

XtFault
XtAggregateStream::StartSlaveBuffer()
{
  XtFault fault;
  for(size_t i = 0; i < _streams.size(); i++)
  {
    _rings[i].input.Clear();
    _rings[i].output.Clear();
  }

  auto guard = XtiGuard([this] { StopSlaveBuffer(); });
  for(size_t i = 0; i < _streams.size(); i++)
    if(i != static_cast<size_t>(_masterIndex))
      if((fault = _streams[i]->StartSlaveBuffer()) != 0) return fault;
  if((fault = _streams[_masterIndex]->StartSlaveBuffer()) != 0) return fault;
  guard.Commit();
  return 0;
}

XtFault
XtAggregateStream::GetLatency(XtLatency* latency) const 
{
  XtFault fault;
  XtLatency local = { 0 };
  auto invRate = 1000.0 / _adapter->_params.format.mix.rate;
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

uint32_t XT_CALLBACK 
XtAggregateStream::OnSlaveBuffer(XtStream const* stream, XtBuffer const* buffer, void* user)
{
  return 0;
}

uint32_t XT_CALLBACK 
XtAggregateStream::OnMasterBuffer(XtStream const* stream, XtBuffer const* buffer, void* user)
{
  return 0;
}
#endif