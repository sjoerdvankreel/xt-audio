#if XT_ENABLE_WASAPI
#include <xt/shared/Win32.hpp>
#include <xt/backend/wasapi/Shared.hpp>
#include <xt/backend/wasapi/Private.hpp>

#include <cmath>
#include <memory>
#include <algorithm>

XtFault 
WasapiDevice::ShowControlPanel()
{ return S_OK; }
void*
WasapiDevice::GetHandle() const
{ return _device.p; }
XtFault
WasapiDevice::SupportsAccess(XtBool interleaved, XtBool* supports) const
{ *supports = interleaved; return S_OK; }
XtFault
WasapiDevice::GetChannelName(XtBool output, int32_t index, char* buffer, int32_t* size) const
{ XtiCopyString(XtiWfxChannelNames[index], buffer, size); return S_OK; }

XtFault
WasapiDevice::GetChannelCount(XtBool output, int32_t* count) const
{ 
  bool isOutput = XtiWasapiTypeIsOutput(_type);
  *count = (output != XtFalse) == isOutput? sizeof(XtiWfxChannelNames) / sizeof(const char*): 0;
  return S_OK;
}

XtFault
WasapiDevice::OpenBlockingStream(XtBlockingParams const* params, XtBlockingStream** stream)
{
  HRESULT hr;
  REFERENCE_TIME hwBuffer;
  REFERENCE_TIME engineBuffer;
  XT_VERIFY_COM(_client->GetDevicePeriod(&engineBuffer, &hwBuffer));
  bool exclusive = XtiWasapiTypeIsExclusive(_type);
  auto maxMs = exclusive? XtiWasapiMaxExclusiveBufferMs: XtiWasapiMaxSharedBufferMs;
  REFERENCE_TIME  minBuffer = exclusive? hwBuffer: engineBuffer;
  REFERENCE_TIME maxBuffer = static_cast<REFERENCE_TIME>(maxMs * XtiWasapiHnsPerMs);
  REFERENCE_TIME buffer = static_cast<REFERENCE_TIME>(std::ceil(params->bufferSize * XtiWasapiHnsPerMs));
  buffer = std::clamp(buffer, minBuffer, maxBuffer);

  auto result = std::make_unique<WasapiStream>();
  result->_type = _type;
  auto pclient = reinterpret_cast<void**>(&result->_client);
  XT_VERIFY_COM(_device->Activate(__uuidof(IAudioClient), CLSCTX_ALL, nullptr, pclient));
  XT_VERIFY_COM(InitializeStream(params, buffer, result.get()));
  XT_VERIFY_COM(result->_client->GetBufferSize(&result->_frames));

  auto pclock = reinterpret_cast<void**>(&result->_clock);
  XT_VERIFY_COM(result->_client->GetService(__uuidof(IAudioClock), pclock));
  auto pclock2 = reinterpret_cast<void**>(&result->_clock2);
  XT_VERIFY_COM(result->_clock->QueryInterface(__uuidof(IAudioClock2), pclock2));

  bool output = XtiWasapiTypeIsOutput(_type);
  auto prender = reinterpret_cast<void**>(&result->_render);
  auto pcapture = reinterpret_cast<void**>(&result->_capture);
  if(output) XT_VERIFY_COM(result->_client->GetService(__uuidof(IAudioRenderClient), prender));
  else XT_VERIFY_COM(result->_client->GetService(__uuidof(IAudioCaptureClient), pcapture));
  *stream = result.release();
  return S_OK;
}

#endif // XT_ENABLE_WASAPI