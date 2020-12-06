#if XT_ENABLE_WASAPI
#include <xt/private/Win32.hpp>
#include <xt/api/private/Service.hpp>
#include <xt/api/private/Device.hpp>
#include <xt/api/private/Stream.hpp>
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

struct XtEvent
{
  HANDLE event;
  XtEvent(XtEvent const&) = delete;
  XtEvent& operator=(XtEvent const&) = delete;
  ~XtEvent() { XT_ASSERT(CloseHandle(event)); }
  XtEvent(): event() { XT_ASSERT((event = CreateEvent(nullptr, FALSE, FALSE, nullptr)) != nullptr); }
};

struct Options {
  bool output;
  bool loopback;
  bool exclusive;
};

static const double XtWsHnsPerMs = 10000.0;
static const double XtWsMaxSharedBufferMs = 2000.0;
static const double XtWsMaxExclusiveBufferMs = 500.0;

// ---- forward ----

struct WasapiService: public XtService 
{
  XT_IMPLEMENT_SERVICE(WASAPI);
};

std::unique_ptr<XtService>
XtiCreateWasapiService()
{ return std::make_unique<WasapiService>(); }

struct WasapiDevice: public XtDevice {
  const Options options;
  const CComPtr<IMMDevice> device;
  const CComPtr<IAudioClient> client;
  const CComPtr<IAudioClient3> client3;
  XT_IMPLEMENT_DEVICE(WASAPI);
  
  WasapiDevice(CComPtr<IMMDevice> d, CComPtr<IAudioClient> c, CComPtr<IAudioClient3> c3, const Options& o):
  XtDevice(), options(o), device(d), client(c), client3(c3) {}
};

struct WasapiStream: public XtBlockingStream {
  HANDLE mmcssHandle;
  UINT32 bufferFrames;
  const Options options;
  const XtEvent streamEvent;
  const CComPtr<IAudioClock> clock;
  const CComPtr<IAudioClock2> clock2;
  const CComPtr<IAudioClient> client;
  const CComPtr<IAudioClient> loopback;
  const CComPtr<IAudioRenderClient> render;
  const CComPtr<IAudioCaptureClient> capture;
  XT_IMPLEMENT_BLOCKING_STREAM(WASAPI);

  ~WasapiStream() {  }
  WasapiStream(bool secondary, UINT32 bufferFrames, CComPtr<IAudioClock> clock, CComPtr<IAudioClock2> clock2, 
    CComPtr<IAudioClient> client, CComPtr<IAudioClient> loopback, CComPtr<IAudioCaptureClient> capture,
    CComPtr<IAudioRenderClient> render, const Options& options):
  XtBlockingStream(secondary), mmcssHandle(), bufferFrames(bufferFrames),
  options(options), streamEvent(), clock(clock), clock2(clock2), 
  client(client), loopback(loopback), render(render), capture(capture) {}

  const wchar_t* GetMmcssTaskName() const;
};

// ---- local ----

static HRESULT GetDevices(
  CComPtr<IMMDeviceCollection>& ins, UINT& ic, CComPtr<IMMDeviceCollection>& outs, UINT& oc) {  

  HRESULT hr;
  CComPtr<IMMDeviceEnumerator> enumerator;
  XT_VERIFY_COM(enumerator.CoCreateInstance(__uuidof(MMDeviceEnumerator)));
  XT_VERIFY_COM(enumerator->EnumAudioEndpoints(eCapture, DEVICE_STATE_ACTIVE, &ins));
  XT_VERIFY_COM(enumerator->EnumAudioEndpoints(eRender, DEVICE_STATE_ACTIVE, &outs));
  XT_VERIFY_COM(ins->GetCount(&ic));
  XT_VERIFY_COM(outs->GetCount(&oc));
  return S_OK;
}

// ---- service -----

XtCapabilities WasapiService::GetCapabilities() const {
  return static_cast<XtCapabilities>(
    XtCapabilitiesTime | 
    XtCapabilitiesLatency | 
    XtCapabilitiesAggregation |
    XtCapabilitiesChannelMask |
    XtCapabilitiesXRunDetection);
}

XtFault WasapiService::GetDeviceCount(int32_t* count) const {
  HRESULT hr;
  UINT inCount, outCount;
  CComPtr<IMMDeviceCollection> inputs, outputs;
  XT_VERIFY_COM(GetDevices(inputs, inCount, outputs, outCount));
  *count = 2 * inCount + 3 * outCount;
  return S_OK;
}

XtFault WasapiService::OpenDefaultDevice(XtBool output, XtDevice** device) const {
  HRESULT hr;
  Options options;
  CComPtr<IMMDevice> d;
  CComPtr<IAudioClient> client;
  CComPtr<IAudioClient3> client3;
  CComPtr<IMMDeviceEnumerator> enumerator;

  *device = nullptr;
  options.loopback = false;
  options.exclusive = false;
  options.output = output != XtFalse;
  XT_VERIFY_COM(enumerator.CoCreateInstance(__uuidof(MMDeviceEnumerator)));
  hr = enumerator->GetDefaultAudioEndpoint(output? eRender: eCapture, eMultimedia, &d);
  if(hr == E_NOTFOUND)
    return 0;
  XT_VERIFY_COM(hr);
  XT_VERIFY_COM(d->Activate(__uuidof(IAudioClient), CLSCTX_ALL, nullptr, reinterpret_cast<void**>(&client)));
  if(!options.loopback) {
    hr = client.QueryInterface(&client3);
    if(hr != E_NOINTERFACE)
      XT_VERIFY_COM(hr);
  }
  *device = new WasapiDevice(d, client, client3, options);
  return S_OK;
}

XtFault
WasapiService::OpenDeviceList(XtDeviceList** list) const { 
  return 0; 
}

XtFault WasapiService::OpenDevice(int32_t index, XtDevice** device) const {  
  HRESULT hr;
  CComPtr<IMMDevice> d;
  UINT inCount, outCount;
  Options options = { 0 };
  CComPtr<IAudioClient> client;  
  CComPtr<IAudioClient3> client3;
  CComPtr<IMMDeviceCollection> inputs, outputs;
  uint32_t uindex = static_cast<uint32_t>(index);

  XT_VERIFY_COM(GetDevices(inputs, inCount, outputs, outCount));
  if(uindex < inCount) {
    options.output = false;
    options.loopback = false;
    options.exclusive = false;
    XT_VERIFY_COM(inputs->Item(uindex, &d));
  } else if(uindex < inCount + outCount) {
    options.output = false;
    options.loopback = true;
    options.exclusive = false;
    XT_VERIFY_COM(outputs->Item(uindex - inCount, &d));
  } else if(uindex < 2 * inCount + outCount) {
    options.output = false;
    options.loopback = false;
    options.exclusive = true;
    XT_VERIFY_COM(inputs->Item(uindex - inCount - outCount, &d));
  } else if(uindex < 2 * inCount + 2 * outCount) {
    options.output = true;
    options.loopback = false;
    options.exclusive = false;
    XT_VERIFY_COM(outputs->Item(uindex - 2 * inCount - outCount, &d));
  } else {
    options.output = true;
    options.loopback = false;
    options.exclusive = true;
    XT_VERIFY_COM(outputs->Item(uindex - 2 * inCount - 2 * outCount , &d));
  }

  XT_VERIFY_COM(d->Activate(__uuidof(IAudioClient), CLSCTX_ALL, nullptr, reinterpret_cast<void**>(&client)));
  if(!options.loopback) {
    hr = client.QueryInterface(&client3);
    if(hr != E_NOINTERFACE)
      XT_VERIFY_COM(hr);
  }
  *device = new WasapiDevice(d, client, client3, options);
  return S_OK;
}

// ---- device ----

XtFault WasapiDevice::ShowControlPanel() {
  return S_OK;
}

XtFault WasapiDevice::GetChannelName(XtBool output, int32_t index, char* buffer, int32_t* size) const {
  XtiCopyString(XtiWfxChannelNames[index], buffer, size);
  return S_OK;
}

XtFault WasapiDevice::GetChannelCount(XtBool output, int32_t* count) const {
  *count =  (output != XtFalse) != options.output? 0: sizeof(XtiWfxChannelNames) / sizeof(const char*);
  return S_OK;
}

XtFault WasapiDevice::SupportsAccess(XtBool interleaved, XtBool* supports) const {
  *supports = interleaved;
  return S_OK;
}

XtFault WasapiDevice::GetName(char* buffer, int32_t* size) const {  
  HRESULT hr;
  XtPropVariant n;
  std::string result;
  CComPtr<IPropertyStore> store;

  XT_VERIFY_COM(device->OpenPropertyStore(STGM_READ, &store));
  XT_VERIFY_COM(store->GetValue(PKEY_Device_FriendlyName, &n.pv));
  result = XtiWideStringToUtf8(n.pv.pwszVal);
  result.append(options.loopback? " (Loopback)": options.exclusive? " (Exclusive)": " (Shared)");
  XtiCopyString(result.c_str(), buffer, size);
  return S_OK;
}

XtFault WasapiDevice::GetMix(XtBool* valid, XtMix* mix) const {  
  HRESULT hr;
  XtFormat match;
  UINT32 currentFrames;
  CComHeapPtr<WAVEFORMATEX> wfx;

  if(options.exclusive)
    return S_OK;
  if(!client3)
    XT_VERIFY_COM(client->GetMixFormat(&wfx));
  else
    XT_VERIFY_COM(client3->GetCurrentSharedModeEnginePeriod(&wfx, &currentFrames));
  if(XtiWfxToFormat(*wfx, options.output, match)) {
    *valid = XtTrue;
    mix->rate = match.mix.rate;
    mix->sample = match.mix.sample;
    return S_OK;
  }
  return S_OK;
}

XtFault WasapiDevice::GetBufferSize(const XtFormat* format, XtBufferSize* size) const {  
  HRESULT hr;
  WAVEFORMATEXTENSIBLE wfx;
  REFERENCE_TIME engine, hardware;
  UINT32 default_, fundamental, min, max;

  if(options.exclusive) {
    XT_VERIFY_COM(client->GetDevicePeriod(&engine, &hardware));
    size->max = XtWsMaxExclusiveBufferMs;
    size->min = hardware / XtWsHnsPerMs;
    size->current = hardware / XtWsHnsPerMs;
    return S_OK;
  } else if(!client3) {
    XT_VERIFY_COM(client->GetDevicePeriod(&engine, &hardware));
    size->max = XtWsMaxSharedBufferMs;
    size->min = engine / XtWsHnsPerMs;
    size->current = engine / XtWsHnsPerMs;
    return S_OK;
  } else {
    XT_ASSERT(XtiFormatToWfx(*format, wfx));
    XT_VERIFY_COM(client3->GetSharedModeEnginePeriod(reinterpret_cast<const WAVEFORMATEX*>(&wfx), &default_, &fundamental, &min, &max));
    size->min = min * 1000.0 / format->mix.rate;
    size->max = max * 1000.0 / format->mix.rate;
    size->current = default_ * 1000.0 / format->mix.rate;
    return S_OK;
  }
}

XtFault WasapiDevice::SupportsFormat(const XtFormat* format, XtBool* supports) const {  
  HRESULT hr;
  WAVEFORMATEXTENSIBLE wfx;
  CComHeapPtr<WAVEFORMATEX> mix;
  CComHeapPtr<WAVEFORMATEX> match;

  if(format->channels.inputs > 0 && options.output || format->channels.outputs > 0 && !options.output || !XtiFormatToWfx(*format, wfx))
    return S_OK;
  if(options.exclusive)
    hr = client->IsFormatSupported(AUDCLNT_SHAREMODE_EXCLUSIVE, reinterpret_cast<WAVEFORMATEX*>(&wfx), nullptr);
  else
    hr = client->IsFormatSupported(AUDCLNT_SHAREMODE_SHARED, reinterpret_cast<WAVEFORMATEX*>(&wfx), &match);
  *supports = hr == S_OK;
  if(hr != AUDCLNT_E_UNSUPPORTED_FORMAT)
    XT_VERIFY_COM(hr);
  if(options.loopback) {
    XT_VERIFY_COM(client->GetMixFormat(&mix));
    *supports &= mix->nChannels == format->channels.inputs;
  }
  return S_OK;
}

XtFault WasapiDevice::OpenStreamCore(const XtDeviceStreamParams* params, bool secondary, void* user, XtStream** stream) {

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
  result = std::make_unique<WasapiStream>(secondary, bufferFrames, clock, clock2, streamClient, loopbackClient, capture, render, this->options);
  if(this->options.loopback)
    XT_VERIFY_COM(loopbackClient->SetEventHandle(result->streamEvent.event));
  else
    XT_VERIFY_COM(streamClient->SetEventHandle(result->streamEvent.event));
  *stream = result.release();
  return S_OK;
}

// ---- stream ----

void WasapiStream::StopStream() {
  XT_ASSERT(SUCCEEDED(client->Stop()));
  if(loopback)
    XT_ASSERT(SUCCEEDED(loopback->Stop()));
  if(!_secondary) {
    XT_ASSERT(AvRevertMmThreadCharacteristics(mmcssHandle));
    mmcssHandle = nullptr;
  }
}

void WasapiStream::StartStream() {
  if(!_secondary) {
    DWORD taskIndex = 0;
    const wchar_t* mmcssTaskName = options.exclusive? L"Pro Audio": L"Audio";
    XT_ASSERT((mmcssHandle = AvSetMmThreadCharacteristicsW(mmcssTaskName, &taskIndex)) != nullptr);
  }
  XT_ASSERT(SUCCEEDED(client->Start()));
  if(loopback)
    XT_ASSERT(SUCCEEDED(loopback->Start()));
}

XtFault WasapiStream::GetFrames(int32_t* frames) const {
  HRESULT hr;
  UINT bufferFrames;
  XT_VERIFY_COM(client->GetBufferSize(&bufferFrames));
  *frames = bufferFrames;
  return S_OK;
}

XtFault WasapiStream::GetLatency(XtLatency* latency) const {
  HRESULT hr;
  REFERENCE_TIME l;
  UINT bufferFrames;
  XT_VERIFY_COM(client->GetStreamLatency(&l));
  XT_VERIFY_COM(client->GetBufferSize(&bufferFrames));
  double bufferLatency = options.exclusive? 0.0: bufferFrames * 1000.0 / _params.format.mix.rate;
  if(capture)
    latency->input = l / XtWsHnsPerMs + bufferLatency;
  else
    latency->output = l / XtWsHnsPerMs + bufferLatency;
  return S_OK;
}

void WasapiStream::ProcessBuffer(bool prefill) {

  HRESULT hr;
  BYTE* data;
  DWORD flags;
  UINT32 padding;
  uint32_t frames;
  DWORD waitResult;
  double time = 0.0;
  uint64_t frequency;
  uint64_t wasapiTime;
  uint64_t position = 0;
  uint64_t wasapiPosition;
  XtBool timeValid = XtFalse;
  DWORD bufferMillis = static_cast<DWORD>(bufferFrames * 1000.0 / _params.format.mix.rate);
  XtBuffer buffer = { 0 };

  if(!prefill && !_secondary) {
    waitResult = WaitForSingleObject(streamEvent.event, bufferMillis);
    if(waitResult == WAIT_TIMEOUT)
      return;
    XT_ASSERT(waitResult == WAIT_OBJECT_0);
  }

  if(capture && !prefill) {
    hr = capture->GetBuffer(&data, &frames, &flags, &wasapiPosition, &wasapiTime);
    if(hr == AUDCLNT_S_BUFFER_EMPTY) {
      XT_VERIFY_ON_BUFFER(capture->ReleaseBuffer(0));
      return;
    }
    if(!XT_VERIFY_ON_BUFFER(hr))
      return;
    if((flags & AUDCLNT_BUFFERFLAGS_DATA_DISCONTINUITY) != 0)
      OnXRun();
    timeValid = (flags & AUDCLNT_BUFFERFLAGS_TIMESTAMP_ERROR) == 0;
    position = !timeValid? 0: wasapiPosition;
    time = !timeValid? 0: wasapiTime / XtWsHnsPerMs;
    buffer.input = data;
    buffer.output = nullptr;
    buffer.frames = frames;
    buffer.time = time;
    buffer.position = position;
    buffer.timeValid = timeValid;
    OnBuffer(&buffer);
    XT_VERIFY_ON_BUFFER(capture->ReleaseBuffer(frames));
  }
  
  if(render && !options.loopback) {
    if(options.exclusive) {
      frames = bufferFrames;
    } else {
      if(!XT_VERIFY_ON_BUFFER(client->GetCurrentPadding(&padding)))
        return;
      frames = bufferFrames - padding;
    }
    if(!prefill) {
      if(options.exclusive) {
        if(!XT_VERIFY_ON_BUFFER(clock->GetFrequency(&frequency)))
          return;
        if(!XT_VERIFY_ON_BUFFER(clock->GetPosition(&wasapiPosition, &wasapiTime)))
          return;
        position = wasapiPosition * _params.format.mix.rate / frequency;
      } else {
        if(!XT_VERIFY_ON_BUFFER(clock2->GetDevicePosition(&wasapiPosition, &wasapiTime)))
          return;
        position = wasapiPosition;
      }
      timeValid = XtTrue;
      time = wasapiTime / XtWsHnsPerMs; 
    }
    if(!XT_VERIFY_ON_BUFFER(render->GetBuffer(frames, &data)))
      return;
    buffer.input = nullptr;
    buffer.output = data;
    buffer.frames = frames;
    buffer.time = time;
    buffer.position = position;
    buffer.timeValid = timeValid;
    OnBuffer(&buffer);
    XT_VERIFY_ON_BUFFER(render->ReleaseBuffer(frames, 0));
  }
}

#endif // XT_ENABLE_WASAPI