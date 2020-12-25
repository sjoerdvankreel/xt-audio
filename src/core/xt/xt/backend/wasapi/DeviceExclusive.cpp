#if XT_ENABLE_WASAPI
#include <xt/shared/Win32.hpp>
#include <xt/backend/wasapi/Shared.hpp>
#include <xt/backend/wasapi/Private.hpp>

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

XtFault
WasapiExclusiveDevice::OpenBlockingStream(XtBlockingParams const* params, XtBlockingStream** stream)
{
  return S_OK;
}

#endif // XT_ENABLE_WASAPI