#ifdef _WIN32
#include "xt-win32.hpp"

#ifdef XT_DISABLE_WASAPI
const XtService* XtiServiceWasapi = nullptr;
#else // XT_DISABLE_WASAPI

#define INITGUID 1
#include <mmdeviceapi.h>
#include <audioclient.h>
#include <functiondiscoverykeys_devpkey.h>
#include <avrt.h>
#include <memory>
#include <cmath>

// ---- local ----

struct Options {
  bool output;
  bool loopback;
  bool exclusive;
};

static const double XtWsHnsPerMs = 10000.0;
static const double XtWsMaxSharedBufferMs = 2000.0;
static const double XtWsMaxExclusiveBufferMs = 500.0;

// ---- forward ----

XT_DECLARE_SERVICE(Wasapi);

struct WasapiDevice: public XtDevice {
  const Options options;
  const CComPtr<IMMDevice> device;
  const CComPtr<IAudioClient> client;
  const CComPtr<IAudioClient3> client3;
  XT_IMPLEMENT_DEVICE(Wasapi);
  
  WasapiDevice(CComPtr<IMMDevice> d, CComPtr<IAudioClient> c, CComPtr<IAudioClient3> c3, const Options& o):
  XtDevice(), options(o), device(d), client(c), client3(c3) {}
};

struct WasapiStream: public XtwWin32BlockingStream {
  HANDLE mmcssHandle;
  UINT32 bufferFrames;
  const Options options;
  const XtwEvent streamEvent;
  const CComPtr<IAudioClock> clock;
  const CComPtr<IAudioClock2> clock2;
  const CComPtr<IAudioClient> client;
  const CComPtr<IAudioClient> loopback;
  const CComPtr<IAudioRenderClient> render;
  const CComPtr<IAudioCaptureClient> capture;
  XT_IMPLEMENT_BLOCKING_STREAM(Wasapi);

  ~WasapiStream() { Stop(); }
  WasapiStream(bool secondary, UINT32 bufferFrames, CComPtr<IAudioClock> clock, CComPtr<IAudioClock2> clock2, 
    CComPtr<IAudioClient> client, CComPtr<IAudioClient> loopback, CComPtr<IAudioCaptureClient> capture,
    CComPtr<IAudioRenderClient> render, const Options& options):
  XtwWin32BlockingStream(secondary), mmcssHandle(), bufferFrames(bufferFrames),
  options(options), streamEvent(), clock(clock), clock2(clock2), 
  client(client), loopback(loopback), render(render), capture(capture) {}

  const wchar_t* GetMmcssTaskName() const;
};

// ---- win32 ----

XtCause XtwWasapiGetFaultCause(XtFault fault)  {
  switch(fault) {
  case AUDCLNT_E_CPUUSAGE_EXCEEDED: return XtCauseGeneric;
  case AUDCLNT_E_UNSUPPORTED_FORMAT: return XtCauseFormat;
  case AUDCLNT_E_SERVICE_NOT_RUNNING: return XtCauseService;
  case AUDCLNT_E_DEVICE_IN_USE:
  case AUDCLNT_E_DEVICE_INVALIDATED:
  case AUDCLNT_E_ENGINE_FORMAT_LOCKED:
  case AUDCLNT_E_RESOURCES_INVALIDATED:
  case AUDCLNT_E_ENGINE_PERIODICITY_LOCKED:
  case AUDCLNT_E_EXCLUSIVE_MODE_NOT_ALLOWED: return XtCauseEndpoint;
  default: return XtCauseUnknown;
  }
}

const char* XtwWasapiGetFaultText(XtFault fault) {
  switch(fault) {
  case AUDCLNT_E_NOT_STOPPED: return "AUDCLNT_E_NOT_STOPPED";
  case AUDCLNT_E_BUFFER_ERROR: return "AUDCLNT_E_BUFFER_ERROR";
  case AUDCLNT_E_OUT_OF_ORDER: return "AUDCLNT_E_OUT_OF_ORDER";
  case AUDCLNT_E_INVALID_SIZE: return "AUDCLNT_E_INVALID_SIZE";
  case AUDCLNT_E_DEVICE_IN_USE: return "AUDCLNT_E_DEVICE_IN_USE";
  case AUDCLNT_E_NOT_INITIALIZED: return "AUDCLNT_E_NOT_INITIALIZED";
  case AUDCLNT_E_BUFFER_TOO_LARGE: return "AUDCLNT_E_BUFFER_TOO_LARGE";
  case AUDCLNT_E_OFFLOAD_MODE_ONLY: return "AUDCLNT_E_OFFLOAD_MODE_ONLY";
  case AUDCLNT_E_CPUUSAGE_EXCEEDED: return "AUDCLNT_E_CPUUSAGE_EXCEEDED";
  case AUDCLNT_E_BUFFER_SIZE_ERROR: return "AUDCLNT_E_BUFFER_SIZE_ERROR";
  case AUDCLNT_E_DEVICE_INVALIDATED: return "AUDCLNT_E_DEVICE_INVALIDATED";
  case AUDCLNT_E_UNSUPPORTED_FORMAT: return "AUDCLNT_E_UNSUPPORTED_FORMAT";
  case AUDCLNT_E_ALREADY_INITIALIZED: return "AUDCLNT_E_ALREADY_INITIALIZED";
  case AUDCLNT_E_INVALID_STREAM_FLAG: return "AUDCLNT_E_INVALID_STREAM_FLAG";
  case AUDCLNT_E_SERVICE_NOT_RUNNING: return "AUDCLNT_E_SERVICE_NOT_RUNNING";
  case AUDCLNT_E_EVENTHANDLE_NOT_SET: return "AUDCLNT_E_EVENTHANDLE_NOT_SET";
  case AUDCLNT_E_EXCLUSIVE_MODE_ONLY: return "AUDCLNT_E_EXCLUSIVE_MODE_ONLY";
  case AUDCLNT_E_WRONG_ENDPOINT_TYPE: return "AUDCLNT_E_WRONG_ENDPOINT_TYPE";
  case AUDCLNT_E_ENGINE_FORMAT_LOCKED: return "AUDCLNT_E_ENGINE_FORMAT_LOCKED";
  case AUDCLNT_E_RAW_MODE_UNSUPPORTED: return "AUDCLNT_E_RAW_MODE_UNSUPPORTED";
  case AUDCLNT_E_NONOFFLOAD_MODE_ONLY: return "AUDCLNT_E_NONOFFLOAD_MODE_ONLY";
  case AUDCLNT_E_THREAD_NOT_REGISTERED: return "AUDCLNT_E_THREAD_NOT_REGISTERED";
  case AUDCLNT_E_RESOURCES_INVALIDATED: return "AUDCLNT_E_RESOURCES_INVALIDATED";
  case AUDCLNT_E_INVALID_DEVICE_PERIOD: return "AUDCLNT_E_INVALID_DEVICE_PERIOD";
  case AUDCLNT_E_INCORRECT_BUFFER_SIZE: return "AUDCLNT_E_INCORRECT_BUFFER_SIZE";
  case AUDCLNT_E_ENDPOINT_CREATE_FAILED: return "AUDCLNT_E_ENDPOINT_CREATE_FAILED";
  case AUDCLNT_E_BUFFER_SIZE_NOT_ALIGNED: return "AUDCLNT_E_BUFFER_SIZE_NOT_ALIGNED";
  case AUDCLNT_E_OUT_OF_OFFLOAD_RESOURCES: return "AUDCLNT_E_OUT_OF_OFFLOAD_RESOURCES";
  case AUDCLNT_E_EVENTHANDLE_NOT_EXPECTED: return "AUDCLNT_E_EVENTHANDLE_NOT_EXPECTED";
  case AUDCLNT_E_BUFFER_OPERATION_PENDING: return "AUDCLNT_E_BUFFER_OPERATION_PENDING";
  case AUDCLNT_E_ENGINE_PERIODICITY_LOCKED: return "AUDCLNT_E_ENGINE_PERIODICITY_LOCKED";
  case AUDCLNT_E_EXCLUSIVE_MODE_NOT_ALLOWED: return "AUDCLNT_E_EXCLUSIVE_MODE_NOT_ALLOWED";
  case AUDCLNT_E_BUFDURATION_PERIOD_NOT_EQUAL: return "AUDCLNT_E_BUFDURATION_PERIOD_NOT_EQUAL";
  case AUDCLNT_E_ENDPOINT_OFFLOAD_NOT_CAPABLE: return "AUDCLNT_E_ENDPOINT_OFFLOAD_NOT_CAPABLE";
  default: return "Unknown fault.";
  }
}

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

const char* WasapiService::GetName() const {
  return "WASAPI";
}

XtFault WasapiService::GetFormatFault() const {
  return AUDCLNT_E_UNSUPPORTED_FORMAT;
}

XtCause WasapiService::GetFaultCause(XtFault fault) const {
  return XtwWasapiGetFaultCause(fault);
}

const char* WasapiService::GetFaultText(XtFault fault) const {
  return XtwWasapiGetFaultText(fault);
}

XtCapabilities WasapiService::GetCapabilities() const {
  return static_cast<XtCapabilities>(
    XtCapabilitiesTime | 
    XtCapabilitiesLatency | 
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

XtFault WasapiDevice::GetChannelName(XtBool output, int32_t index, char** name) const {
  *name = _strdup(XtwWfxChannelNames[index]);
  return S_OK;
}

XtFault WasapiDevice::GetChannelCount(XtBool output, int32_t* count) const {
  *count =  (output != XtFalse) != options.output? 0: sizeof(XtwWfxChannelNames) / sizeof(const char*);
  return S_OK;
}

XtFault WasapiDevice::SupportsAccess(XtBool interleaved, XtBool* supports) const {
  *supports = interleaved;
  return S_OK;
}

XtFault WasapiDevice::GetName(char** name) const {  
  HRESULT hr;
  XtwPropVariant n;
  std::string result;
  CComPtr<IPropertyStore> store;

  XT_VERIFY_COM(device->OpenPropertyStore(STGM_READ, &store));
  XT_VERIFY_COM(store->GetValue(PKEY_Device_FriendlyName, &n.pv));
  result = XtwWideStringToUtf8(n.pv.pwszVal);
  result.append(options.loopback? " (Loopback)": options.exclusive? " (Exclusive)": " (Shared)");
  *name = _strdup(result.c_str());
  return S_OK;
}

XtFault WasapiDevice::GetMix(XtMix** mix) const {  
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
  if(XtwWfxToFormat(*wfx, options.output, match)) {
    *mix = static_cast<XtMix*>(malloc(sizeof(XtMix)));
    (*mix)->rate = match.mix.rate;
    (*mix)->sample = match.mix.sample;
    return S_OK;
  }
  return S_OK;
}

XtFault WasapiDevice::GetBuffer(const XtFormat* format, XtBuffer* buffer) const {  
  HRESULT hr;
  WAVEFORMATEXTENSIBLE wfx;
  REFERENCE_TIME engine, hardware;
  UINT32 default_, fundamental, min, max;

  if(options.exclusive) {
    XT_VERIFY_COM(client->GetDevicePeriod(&engine, &hardware));
    buffer->max = XtWsMaxExclusiveBufferMs;
    buffer->min = hardware / XtWsHnsPerMs;
    buffer->current = hardware / XtWsHnsPerMs;
    return S_OK;
  } else if(!client3) {
    XT_VERIFY_COM(client->GetDevicePeriod(&engine, &hardware));
    buffer->max = XtWsMaxSharedBufferMs;
    buffer->min = engine / XtWsHnsPerMs;
    buffer->current = engine / XtWsHnsPerMs;
    return S_OK;
  } else {
    XT_ASSERT(XtwFormatToWfx(*format, wfx));
    XT_VERIFY_COM(client3->GetSharedModeEnginePeriod(reinterpret_cast<const WAVEFORMATEX*>(&wfx), &default_, &fundamental, &min, &max));
    buffer->min = min * 1000.0 / format->mix.rate;
    buffer->max = max * 1000.0 / format->mix.rate;
    buffer->current = default_ * 1000.0 / format->mix.rate;
    return S_OK;
  }
}

XtFault WasapiDevice::SupportsFormat(const XtFormat* format, XtBool* supports) const {  
  HRESULT hr;
  WAVEFORMATEXTENSIBLE wfx;
  CComHeapPtr<WAVEFORMATEX> mix;
  CComHeapPtr<WAVEFORMATEX> match;

  if(format->channels.inputs > 0 && options.output || format->channels.outputs > 0 && !options.output || !XtwFormatToWfx(*format, wfx))
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

XtFault WasapiDevice::OpenStream(const XtFormat* format, XtBool interleaved, double bufferSize,
                                 bool secondary, XtStreamCallback callback, void* user, XtStream** stream) {

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

  XT_ASSERT(XtwFormatToWfx(*format, wfx));
  wantedSize = bufferSize * XtWsHnsPerMs;
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
      bufferFrames = static_cast<UINT32>(bufferSize / 1000.0 * format->mix.rate);
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
  if(!secondary) {
    XT_ASSERT(AvRevertMmThreadCharacteristics(mmcssHandle));
    mmcssHandle = nullptr;
  }
}

void WasapiStream::StartStream() {
  if(!secondary) {
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
  double bufferLatency = options.exclusive? 0.0: bufferFrames * 1000.0 / format.mix.rate;
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
  DWORD bufferMillis = static_cast<DWORD>(bufferFrames * 1000.0 / format.mix.rate);

  if(!prefill && !secondary) {
    waitResult = WaitForSingleObject(streamEvent.event, bufferMillis);
    if(waitResult == WAIT_TIMEOUT)
      return;
    XT_ASSERT(waitResult == WAIT_OBJECT_0);
  }

  if(capture && !prefill) {
    hr = capture->GetBuffer(&data, &frames, &flags, &wasapiPosition, &wasapiTime);
    if(hr == AUDCLNT_S_BUFFER_EMPTY) {
      XT_VERIFY_STREAM_CALLBACK(capture->ReleaseBuffer(0));
      return;
    }
    if(!XT_VERIFY_STREAM_CALLBACK(hr))
      return;
    if((flags & AUDCLNT_BUFFERFLAGS_DATA_DISCONTINUITY) != 0)
      ProcessXRun();
    timeValid = (flags & AUDCLNT_BUFFERFLAGS_TIMESTAMP_ERROR) == 0;
    position = !timeValid? 0: wasapiPosition;
    time = !timeValid? 0: wasapiTime / XtWsHnsPerMs;
    ProcessCallback(data, nullptr, frames, time, position, timeValid, 0);
    XT_VERIFY_STREAM_CALLBACK(capture->ReleaseBuffer(frames));
  }
  
  if(render && !options.loopback) {
    if(options.exclusive) {
      frames = bufferFrames;
    } else {
      if(!XT_VERIFY_STREAM_CALLBACK(client->GetCurrentPadding(&padding)))
        return;
      frames = bufferFrames - padding;
    }
    if(!prefill) {
      if(options.exclusive) {
        if(!XT_VERIFY_STREAM_CALLBACK(clock->GetFrequency(&frequency)))
          return;
        if(!XT_VERIFY_STREAM_CALLBACK(clock->GetPosition(&wasapiPosition, &wasapiTime)))
          return;
        position = wasapiPosition * format.mix.rate / frequency;
      } else {
        if(!XT_VERIFY_STREAM_CALLBACK(clock2->GetDevicePosition(&wasapiPosition, &wasapiTime)))
          return;
        position = wasapiPosition;
      }
      timeValid = XtTrue;
      time = wasapiTime / XtWsHnsPerMs; 
    }
    if(!XT_VERIFY_STREAM_CALLBACK(render->GetBuffer(frames, &data)))
      return;
    ProcessCallback(nullptr, data, frames, time, position, timeValid, 0);
    XT_VERIFY_STREAM_CALLBACK(render->ReleaseBuffer(frames, 0));
  }
}

#endif // XT_DISABLE_WASAPI
#endif // _WIN32