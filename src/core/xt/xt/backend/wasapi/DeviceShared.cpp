#if XT_ENABLE_WASAPI
#include <xt/shared/Win32.hpp>
#include <xt/backend/wasapi/Shared.hpp>
#include <xt/backend/wasapi/Private.hpp>

#include <memory>
#include <algorithm>

XtFault
WasapiSharedDevice::GetMix(XtBool* valid, XtMix* mix) const
{ 
  HRESULT hr;
  UINT32 period;
  XtFormat match;
  CComHeapPtr<WAVEFORMATEX> wfx;

  if(!_client3)
    XT_VERIFY_COM(_client->GetMixFormat(&wfx));
  else
    XT_VERIFY_COM(_client3->GetCurrentSharedModeEnginePeriod(&wfx, &period));
  if(XtiWfxToFormat(*wfx, XtiWasapiTypeIsOutput(_type), match))
  {
    *valid = XtTrue;
    mix->rate = match.mix.rate;
    mix->sample = match.mix.sample;
  }
  return S_OK;
}

XtFault
WasapiSharedDevice::SupportsFormat(XtFormat const* format, XtBool* supports) const
{
  HRESULT hr;
  WAVEFORMATEXTENSIBLE wfx;  
  CComHeapPtr<WAVEFORMATEX> mix;
  CComHeapPtr<WAVEFORMATEX> match;
  bool isOutput = XtiWasapiTypeIsOutput(_type);
  if(!XtiFormatToWfx(*format, wfx)) return S_OK;
  if(format->channels.inputs > 0 && isOutput) return S_OK;
  if(format->channels.outputs > 0 && !isOutput) return S_OK;

  auto pwfx = reinterpret_cast<WAVEFORMATEX*>(&wfx);
  hr = _client->IsFormatSupported(AUDCLNT_SHAREMODE_SHARED, pwfx, &match);
  *supports = hr == S_OK;
  if(hr != AUDCLNT_E_UNSUPPORTED_FORMAT) XT_VERIFY_COM(hr);
  if(_type == XtWasapiType::Loopback)
  {
    XT_VERIFY_COM(_client->GetMixFormat(&mix));
    *supports &= mix->nChannels == format->channels.inputs;
  }
  return S_OK;
}

XtFault
WasapiSharedDevice::GetBufferSize(XtFormat const* format, XtBufferSize* size) const
{
  HRESULT hr;
  WAVEFORMATEXTENSIBLE wfx;  
  REFERENCE_TIME engine, hardware;
  UINT32 default_, fundamental, min, max;

  if(!_client3)
  {
    XT_VERIFY_COM(_client->GetDevicePeriod(&engine, &hardware));
    size->max = XtiWasapiMaxSharedBufferMs;
    size->min = engine / XtiWasapiHnsPerMs;
    size->current = engine / XtiWasapiHnsPerMs;
    return S_OK;
  }
  auto pwfx = reinterpret_cast<const WAVEFORMATEX*>(&wfx);
  XT_ASSERT(XtiFormatToWfx(*format, wfx));
  XT_VERIFY_COM(_client3->GetSharedModeEnginePeriod(pwfx, &default_, &fundamental, &min, &max));
  size->min = min * 1000.0 / format->mix.rate;
  size->max = max * 1000.0 / format->mix.rate;
  size->current = default_ * 1000.0 / format->mix.rate;
  return S_OK;
}

HRESULT
WasapiSharedDevice::InitializeStream(XtBlockingParams const* params, REFERENCE_TIME buffer, CComPtr<IAudioClient>& client)
{
  HRESULT hr; 
  WAVEFORMATEXTENSIBLE wfx;
  XT_ASSERT(XtiFormatToWfx(params->format, wfx));
  auto format = reinterpret_cast<WAVEFORMATEX*>(&wfx);
  auto flags = _type == XtWasapiType::Loopback? AUDCLNT_STREAMFLAGS_LOOPBACK: AUDCLNT_STREAMFLAGS_EVENTCALLBACK;

  if(!_client3)
  {
    auto mode = AUDCLNT_SHAREMODE_SHARED;
    XT_VERIFY_COM(client->Initialize(mode, flags, buffer, 0, format, nullptr));
    return S_OK;
  }


  auto ploopback = reinterpret_cast<void**>(&result->_loopback);
  XT_VERIFY_COM(_device->Activate(__uuidof(IAudioClient), CLSCTX_ALL, nullptr, ploopback));
  if(!_client3) XT_VERIFY_COM(result->_loopback->Initialize(AUDCLNT_SHAREMODE_SHARED, AUDCLNT_STREAMFLAGS_EVENTCALLBACK, buffer, 0, pWfx, nullptr));
  else
  {
    XT_VERIFY_COM(loopbackClient.QueryInterface(&loopbackClient3));
    XT_VERIFY_COM(loopbackClient3->InitializeSharedAudioStream(AUDCLNT_STREAMFLAGS_EVENTCALLBACK, bufferFrames, pWfx, nullptr));
  }
  XT_VERIFY_COM(loopbackClient->SetEventHandle(result->streamEvent.event));
  *stream = result.release();


  CComPtr<IAudioClient3> client3;
  UINT min, max, default_, fundamental;
  XT_VERIFY_COM(client->QueryInterface(&client3));
  XT_VERIFY_COM(client3->GetSharedModeEnginePeriod(format, &default_, &fundamental, &min, &max));
  UINT32 frames = static_cast<UINT32>(params->bufferSize / 1000.0 * params->format.mix.rate);
  frames = std::clamp(frames, min, max);
  XT_VERIFY_COM(client3->InitializeSharedAudioStream(flags, frames, format, nullptr));
  return S_OK;
}

#endif // XT_ENABLE_WASAPI