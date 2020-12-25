#if 0
#if XT_ENABLE_WASAPI
#include <xt/private/Win32.hpp>
#include <xt/api/private/Service.hpp>
#include <xt/api/private/Device.hpp>
#include <xt/api/private/Stream.hpp>
#include <xt/api/private/DeviceList.hpp>
#include <xt/private/BlockingStream.hpp>
#define INITGUID 1
#include <xt/private/Shared.hpp>
#include <xt/private/Services.hpp>
#include <mmdeviceapi.h>
#include <audioclient.h>
#include <functiondiscoverykeys_devpkey.h>
#include <avrt.h>
#include <memory>
#include <cmath>

// ---- local ----

// ---- forward ----

// ---- local ----

// ---- device ----

XtFault WasapiDevice::OpenStreamCore(const XtDeviceStreamParams* params, void* user, XtStream** stream) {

  HRESULT hr;
  DWORD flags;
  UINT alignedSize;
  double wantedSize;
  UINT32 bufferFrames;
  AUDCLNT_SHAREMODE mode;
  WAVEFORMATEXTENSIBLE wfx;
  CComPtr<IAudioClock> clock;
  CComPtr<IAudioClock2> clock2;
  REFERENCE_TIME wasapiBufferSize;
  REFERENCE_TIME engine, hardware;
  CComPtr<IAudioRenderClient> render;
  REFERENCE_TIME minBuffer, maxBuffer;
  CComPtr<IAudioClient> streamClient;
  CComPtr<IAudioClient3> streamClient3;
  CComPtr<IAudioCaptureClient> capture;
  std::unique_ptr<WasapiStream> result;
  CComPtr<IAudioClient> loopbackClient;
  CComPtr<IAudioClient3> loopbackClient3;
  UINT32 min, max, default_, fundamental;
  auto pWfx = reinterpret_cast<WAVEFORMATEX*>(&wfx);
  auto pStreamClient = reinterpret_cast<void**>(&streamClient);

  XT_ASSERT(XtiFormatToWfx(params->format, wfx));
  wantedSize = params->bufferSize * XtWsHnsPerMs;
  XT_VERIFY_COM(client->GetDevicePeriod(&engine, &hardware));
  if(this->options.exclusive) {
    minBuffer = hardware;
    maxBuffer = static_cast<REFERENCE_TIME>(XtWsMaxExclusiveBufferMs * XtWsHnsPerMs);
  } else {
    minBuffer = engine;
    maxBuffer = static_cast<REFERENCE_TIME>(XtWsMaxSharedBufferMs * XtWsHnsPerMs);
  }

  wasapiBufferSize = static_cast<REFERENCE_TIME>(std::ceil(wantedSize));
  if(wasapiBufferSize < minBuffer)
    wasapiBufferSize = minBuffer;
  else if(wasapiBufferSize > maxBuffer)
    wasapiBufferSize = maxBuffer;

  XT_VERIFY_COM(device->Activate(__uuidof(IAudioClient), CLSCTX_ALL, nullptr, pStreamClient));
  if(this->options.exclusive) {
    mode = AUDCLNT_SHAREMODE_EXCLUSIVE;
    flags = AUDCLNT_STREAMFLAGS_EVENTCALLBACK;
    hr = streamClient->Initialize(mode, flags, wasapiBufferSize, wasapiBufferSize, pWfx, nullptr);
    while(hr == E_INVALIDARG && wasapiBufferSize > minBuffer) {
      wasapiBufferSize /= 2;
      hr = streamClient->Initialize(mode, flags, wasapiBufferSize, wasapiBufferSize, pWfx, nullptr);
    }
    if(hr != AUDCLNT_E_BUFFER_SIZE_NOT_ALIGNED)
      XT_VERIFY_COM(hr);
    else {
      XT_VERIFY_COM(streamClient->GetBufferSize(&alignedSize));
      wasapiBufferSize = static_cast<REFERENCE_TIME>((1000.0 * XtWsHnsPerMs / wfx.Format.nSamplesPerSec * alignedSize) + 0.5);
      streamClient.Release();
      if(wasapiBufferSize > maxBuffer)
        wasapiBufferSize /= 2;
      if(wasapiBufferSize < minBuffer)
        wasapiBufferSize *= 2;
      XT_VERIFY_COM(device->Activate(__uuidof(IAudioClient), CLSCTX_ALL, nullptr, pStreamClient));
      hr = streamClient->Initialize(mode, flags, wasapiBufferSize, wasapiBufferSize, pWfx, nullptr);
      if(hr != AUDCLNT_E_BUFFER_SIZE_NOT_ALIGNED)
        XT_VERIFY_COM(hr);
      else {
        XT_VERIFY_COM(streamClient->GetBufferSize(&alignedSize));
        wasapiBufferSize = static_cast<REFERENCE_TIME>((1000.0 * XtWsHnsPerMs / wfx.Format.nSamplesPerSec * alignedSize) + 0.5);
        streamClient.Release();
        XT_VERIFY_COM(device->Activate(__uuidof(IAudioClient), CLSCTX_ALL, nullptr, pStreamClient));
        XT_VERIFY_COM(streamClient->Initialize(mode, flags, wasapiBufferSize, wasapiBufferSize, pWfx, nullptr));
      }
    }
  } else {
    mode = AUDCLNT_SHAREMODE_SHARED;
    flags = this->options.loopback? AUDCLNT_STREAMFLAGS_LOOPBACK: AUDCLNT_STREAMFLAGS_EVENTCALLBACK;
    if(!client3) {
      XT_VERIFY_COM(streamClient->Initialize(mode, flags, wasapiBufferSize, 0, pWfx, nullptr));
    } else {
      XT_VERIFY_COM(streamClient.QueryInterface(&streamClient3));
      XT_VERIFY_COM(streamClient3->GetSharedModeEnginePeriod(pWfx, &default_, &fundamental, &min, &max));
      bufferFrames = static_cast<UINT32>(params->bufferSize / 1000.0 * params->format.mix.rate);
      if(bufferFrames < min)
        bufferFrames = min;
      if(bufferFrames > max)
        bufferFrames = max;
      XT_VERIFY_COM(streamClient3->InitializeSharedAudioStream(flags, bufferFrames, pWfx, nullptr));
    }
  }

  XT_VERIFY_COM(streamClient->GetBufferSize(&bufferFrames));
  XT_VERIFY_COM(streamClient->GetService(__uuidof(IAudioClock), reinterpret_cast<void**>(&clock)));
  XT_VERIFY_COM(clock->QueryInterface(__uuidof(IAudioClock2), reinterpret_cast<void**>(&clock2)));
  if(this->options.output && !this->options.loopback)
    XT_VERIFY_COM(streamClient->GetService(__uuidof(IAudioRenderClient), reinterpret_cast<void**>(&render)));
  if(!this->options.output || this->options.loopback)
    XT_VERIFY_COM(streamClient->GetService(__uuidof(IAudioCaptureClient), reinterpret_cast<void**>(&capture)));
  if(this->options.loopback) {
    XT_VERIFY_COM(device->Activate(__uuidof(IAudioClient), CLSCTX_ALL, nullptr, reinterpret_cast<void**>(&loopbackClient)));
    if(!client3) {
      XT_VERIFY_COM(loopbackClient->Initialize(AUDCLNT_SHAREMODE_SHARED, AUDCLNT_STREAMFLAGS_EVENTCALLBACK, wasapiBufferSize, 0, pWfx, nullptr));
    } else {
      XT_VERIFY_COM(loopbackClient.QueryInterface(&loopbackClient3));
      XT_VERIFY_COM(loopbackClient3->InitializeSharedAudioStream(AUDCLNT_STREAMFLAGS_EVENTCALLBACK, bufferFrames, pWfx, nullptr));
    }
  }
  result = std::make_unique<WasapiStream>(bufferFrames, clock, clock2, streamClient, loopbackClient, capture, render, this->options);
  if(this->options.loopback)
    XT_VERIFY_COM(loopbackClient->SetEventHandle(result->streamEvent.event));
  else
    XT_VERIFY_COM(streamClient->SetEventHandle(result->streamEvent.event));
  *stream = result.release();
  return S_OK;
}

#endif // XT_ENABLE_WASAPI
#endif