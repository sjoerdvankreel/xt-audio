#if XT_ENABLE_WASAPI
#include <xt/backend/wasapi/Shared.hpp>
#include <xt/backend/wasapi/Private.hpp>
#include <avrt.h>

void*
WasapiStream::GetHandle() const
{ return _client.p; }

XtFault
WasapiStream::GetFrames(int32_t* frames) const 
{
  HRESULT hr;
  UINT uframes;
  XT_VERIFY_COM(_client->GetBufferSize(&uframes));
  *frames = uframes;
  return S_OK;
}

XtFault
WasapiStream::StartSlaveBuffer()
{
  HRESULT hr;
  XT_VERIFY_COM(_client->Start());
  if(_loopback) XT_VERIFY_COM(_loopback->Start());
  return S_OK;
}

void
WasapiStream::StopSlaveBuffer()
{
  XT_TRACE_IF(FAILED(_client->Stop()));
  if(_loopback) XT_TRACE_IF(FAILED(_loopback->Stop()));
}

void
WasapiStream::StopMasterBuffer()
{
  if(_mmcssHandle == nullptr) return;
  XT_TRACE_IF(!AvRevertMmThreadCharacteristics(_mmcssHandle));
  _mmcssHandle = nullptr;
}

XtFault
WasapiStream::StartMasterBuffer()
{
  DWORD taskIndex = 0;
  bool exclusive = XtiWasapiTypeIsExclusive(_type);
  wchar_t const* taskName = exclusive? L"Pro Audio": L"Audio";
  _mmcssHandle = AvSetMmThreadCharacteristicsW(taskName, &taskIndex);
  XT_VERIFY(_mmcssHandle != nullptr, AUDCLNT_E_BUFFER_ERROR);
  return S_OK;
}

XtFault
WasapiStream::GetLatency(XtLatency* latency) const
{
  HRESULT hr;
  UINT frames;
  REFERENCE_TIME rt;
  auto rate = _params.format.mix.rate;
  XT_VERIFY_COM(_client->GetStreamLatency(&rt));
  XT_VERIFY_COM(_client->GetBufferSize(&frames));
  bool exclusive = XtiWasapiTypeIsExclusive(_type);
  double bufferLatency = exclusive? 0.0: frames * 1000.0 / rate;
  double result = rt / XtiWasapiHnsPerMs + bufferLatency;
  if(_capture) latency->input = result;
  else latency->output = result;
  return S_OK;
}

XtFault
WasapiStream::BlockMasterBuffer()
{
  auto rate = _params.format.mix.rate;
  DWORD bufferMillis = static_cast<DWORD>(_frames * 1000.0 / rate);
  XT_VERIFY(WaitForSingleObject(_event.event, bufferMillis) == WAIT_OBJECT_0, AUDCLNT_E_BUFFER_ERROR);
  return S_OK;
}

XtFault
WasapiStream::PrefillOutputBuffer()
{
  HRESULT hr;
  BYTE* data;
  UINT padding;
  UINT frames = _frames;
  XtBuffer buffer = { 0 };

  if(!XtiWasapiTypeIsOutput(_type)) return S_OK;
  bool exclusive = XtiWasapiTypeIsExclusive(_type);
  if(!exclusive)
  {
    XT_VERIFY_COM(_client->GetCurrentPadding(&padding));
    frames -= padding;
  }
  XT_VERIFY_COM(_render->GetBuffer(frames, &data));
  buffer.output = data;
  buffer.frames = frames;
  OnBuffer(_params.index, &buffer);
  XT_VERIFY_COM(_render->ReleaseBuffer(frames, 0));
  return S_OK;
}  

XtFault 
WasapiStream::ProcessBuffer()
{ 
  BYTE* data;
  HRESULT hr; 
  DWORD flags;
  UINT64 time;
  UINT32 frames;
  UINT32 padding;
  UINT64 position;
  UINT64 frequency;
  XtBuffer buffer = { 0 };

  if(_capture)
  {
    XT_VERIFY_COM(_capture->GetBuffer(&data, &frames, &flags, &position, &time));
    if(hr == AUDCLNT_S_BUFFER_EMPTY)
    {
      XT_VERIFY_COM(_capture->ReleaseBuffer(0));
      return S_OK;
    }
    if((flags & AUDCLNT_BUFFERFLAGS_DATA_DISCONTINUITY) != 0) OnXRun(_params.index);
    buffer.input = data;
    buffer.frames = frames;
    buffer.timeValid = (flags & AUDCLNT_BUFFERFLAGS_TIMESTAMP_ERROR) == 0;
    buffer.position = buffer.timeValid? position: 0;
    buffer.time = buffer.timeValid? time / XtiWasapiHnsPerMs: 0;
    OnBuffer(_params.index, &buffer);
    XT_VERIFY_COM(_capture->ReleaseBuffer(frames));
    return S_OK;
  }
  
  frames = _frames;
  if(XtiWasapiTypeIsExclusive(_type))
  {
    auto rate = _params.format.mix.rate;
    XT_VERIFY_COM(_clock->GetFrequency(&frequency));
    XT_VERIFY_COM(_clock->GetPosition(&position, &time));
    buffer.position = position * rate / frequency;
  } else
  {
    XT_VERIFY_COM(_clock2->GetDevicePosition(&position, &time));
    XT_VERIFY_COM(_client->GetCurrentPadding(&padding));
    frames -= padding;
    buffer.position = position;
  }
  XT_VERIFY_COM(_render->GetBuffer(frames, &data));
  buffer.output = data;
  buffer.frames = frames;
  buffer.timeValid = XtTrue;
  buffer.time = time / XtiWasapiHnsPerMs;
  OnBuffer(_params.index, &buffer);
  XT_VERIFY_COM(_render->ReleaseBuffer(frames, 0));
  return S_OK;
}

#endif // XT_ENABLE_WASAPI