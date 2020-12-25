#if XT_ENABLE_WASAPI
#include <xt/shared/Win32.hpp>
#include <xt/backend/wasapi/Shared.hpp>
#include <xt/backend/wasapi/Private.hpp>
#include <algorithm>

XtFault
WasapiExclusiveDevice::GetMix(XtBool* valid, XtMix* mix) const
{ return S_OK; }

XtFault
WasapiExclusiveDevice::SupportsFormat(XtFormat const* format, XtBool* supports) const
{
  HRESULT hr;
  WAVEFORMATEXTENSIBLE wfx;
  bool isOutput = XtiWasapiTypeIsOutput(_type);
  if(!XtiFormatToWfx(*format, wfx)) return S_OK;
  if(format->channels.inputs > 0 && isOutput) return S_OK;
  if(format->channels.outputs > 0 && !isOutput) return S_OK;

  auto pwfx = reinterpret_cast<WAVEFORMATEX*>(&wfx);
  hr = _client->IsFormatSupported(AUDCLNT_SHAREMODE_EXCLUSIVE, pwfx, nullptr);
  *supports = hr == S_OK;
  if(hr != AUDCLNT_E_UNSUPPORTED_FORMAT) XT_VERIFY_COM(hr);
  return S_OK;
}

XtFault
WasapiExclusiveDevice::GetBufferSize(XtFormat const* format, XtBufferSize* size) const
{
  HRESULT hr;
  REFERENCE_TIME engine, hardware;
  XT_VERIFY_COM(_client->GetDevicePeriod(&engine, &hardware));
  size->max = XtiWasapiMaxExclusiveBufferMs;
  size->min = hardware / XtiWasapiHnsPerMs;
  size->current = hardware / XtiWasapiHnsPerMs;
  return S_OK;
}

HRESULT
WasapiExclusiveDevice::InitializeStream(XtBlockingParams const* params, REFERENCE_TIME buffer, WasapiStream* stream)
{
  HRESULT hr;
  UINT32 aligned;
  WAVEFORMATEXTENSIBLE wfx;
  REFERENCE_TIME hwBuffer;
  REFERENCE_TIME engineBuffer;
  auto mode = AUDCLNT_SHAREMODE_EXCLUSIVE;
  auto flags = AUDCLNT_STREAMFLAGS_EVENTCALLBACK;
  REFERENCE_TIME maxBuffer = static_cast<REFERENCE_TIME>(XtiWasapiMaxExclusiveBufferMs * XtiWasapiHnsPerMs);

  XT_ASSERT(XtiFormatToWfx(params->format, wfx));
  auto format = reinterpret_cast<WAVEFORMATEX*>(&wfx);  
  XT_VERIFY_COM(_client->GetDevicePeriod(&engineBuffer, &hwBuffer));

  hr = stream->_client->Initialize(mode, flags, buffer, buffer, format, nullptr);
  while(hr == E_INVALIDARG && buffer > hwBuffer)
  {
    buffer /= 2;
    hr = stream->_client->Initialize(mode, flags, buffer, buffer, format, nullptr);
  }
  if(hr != AUDCLNT_E_BUFFER_SIZE_NOT_ALIGNED)
  {
    XT_VERIFY_COM(hr);
    XT_VERIFY_COM(stream->_client->SetEventHandle(stream->_event.event));
    return S_OK;
  }

  XT_VERIFY_COM(stream->_client->GetBufferSize(&aligned));
  stream->_client.Release();
  auto alignedRt = (1000.0 * XtiWasapiHnsPerMs / wfx.Format.nSamplesPerSec * aligned) + 0.5;
  buffer = static_cast<REFERENCE_TIME>(alignedRt);
  buffer = std::clamp(buffer, hwBuffer, maxBuffer);
  auto pclient = reinterpret_cast<void**>(&stream->_client);
  XT_VERIFY_COM(_device->Activate(__uuidof(IAudioClient), CLSCTX_ALL, nullptr, pclient));
  hr = stream->_client->Initialize(mode, flags, buffer, buffer, format, nullptr);
  if(hr != AUDCLNT_E_BUFFER_SIZE_NOT_ALIGNED)
  {
    XT_VERIFY_COM(hr);    
    XT_VERIFY_COM(stream->_client->SetEventHandle(stream->_event.event));
    return S_OK;
  }

  stream->_client.Release();
  buffer = static_cast<REFERENCE_TIME>(alignedRt);  
  pclient = reinterpret_cast<void**>(&stream->_client);
  XT_VERIFY_COM(_device->Activate(__uuidof(IAudioClient), CLSCTX_ALL, nullptr, pclient));
  XT_VERIFY_COM(stream->_client->Initialize(mode, flags, buffer, buffer, format, nullptr));
  XT_VERIFY_COM(stream->_client->SetEventHandle(stream->_event.event));
  return S_OK;
}

#endif // XT_ENABLE_WASAPI