#ifdef _WIN32
#include "XtWin32.hpp"

#ifdef XT_DISABLE_DIRECT_SOUND
const XtService* XtiServiceDirectSound = nullptr;
#else // XT_DISABLE_DIRECT_SOUND

#define INITGUID 1
#include <dsound.h>
#include <mmdeviceapi.h>
#include <audioclient.h>
#include <vector>
#include <memory>

// ---- local ----

static const int XtDsWakeUpsPerBuffer = 8;
static const double XtDsMinBufferMs = 100.0;
static const double XtDsMaxBufferMs = 5000.0;
static const double XtDsDefaultBufferMs = 500.0;

// ---- forward ----

XT_DECLARE_SERVICE(DirectSound);

struct DirectSoundDevice: public XtDevice {
  const GUID guid;
  const std::string name;
  const CComPtr<IDirectSound> output;
  const CComPtr<IDirectSoundCapture> input;
  XT_IMPLEMENT_DEVICE(DirectSound);

  DirectSoundDevice(
    const GUID& g, const std::string& n, 
    CComPtr<IDirectSoundCapture> i, CComPtr<IDirectSound> o):
  XtDevice(), guid(g), name(n), output(o), input(i) {}
};

struct DirectSoundStream: public XtwWin32BlockingStream {
  const int32_t frameSize;
  std::vector<char> buffer;
  uint64_t xtBytesProcessed;
  uint64_t dsBytesProcessed;
  int32_t previousDsPosition;
  const int32_t bufferFrames;
  const XtwWaitableTimer timer;
  const CComPtr<IDirectSound> output;
  const CComPtr<IDirectSoundCapture> input;
  const CComPtr<IDirectSoundBuffer> render;
  const CComPtr<IDirectSoundCaptureBuffer> capture;
  XT_IMPLEMENT_BLOCKING_STREAM(DirectSound);

  ~DirectSoundStream() { Stop(); }
  DirectSoundStream(bool secondary,
    CComPtr<IDirectSoundCapture> input, CComPtr<IDirectSound> output,
    CComPtr<IDirectSoundCaptureBuffer> capture, CComPtr<IDirectSoundBuffer> render, 
    int32_t bufferFrames, int32_t frameSize):
  XtwWin32BlockingStream(secondary), frameSize(frameSize),
  buffer(static_cast<size_t>(bufferFrames * frameSize), '\0'),
  xtBytesProcessed(0), dsBytesProcessed(0),
  previousDsPosition(0), bufferFrames(bufferFrames), timer(),
  output(output), input(input), render(render), capture(capture) {}
};

// ---- local ----

struct DeviceInfo {
  GUID guid;
  bool output;
  std::string name;
};

struct EnumContext {
  bool output;
  bool defaults;
  std::vector<DeviceInfo>* infos;
};

static DWORD WrapAround(int32_t bytes, int32_t bufferSize) {
  return bytes >= 0? bytes: bytes + bufferSize;
}

static bool InsideSafetyGap(DWORD read, DWORD write, DWORD lockPosition) {
  return read < write && read <= lockPosition && lockPosition < write ||
         read > write && (read <= lockPosition || lockPosition < write);
}

static UINT GetTimerPeriod(int32_t bufferFrames, int32_t rate) {
  return static_cast<UINT>(bufferFrames * 1000.0 / rate / XtDsWakeUpsPerBuffer);
}

static void SplitBufferParts(
  std::vector<char>& buffer, void* part1, DWORD size1, void* part2, DWORD size2) {

  memcpy(part1, &buffer[0], size1);
  if(size2 != 0)
    memcpy(part2, &buffer[size1], size2);
}

static void CombineBufferParts(
  std::vector<char>& buffer, void* part1, DWORD size1, void* part2, DWORD size2) {

  memcpy(&buffer[0], part1, size1);
  if(size2 != 0)
    memcpy(&buffer[size1], part2, size2);
}

static BOOL CALLBACK EnumCallback(GUID* guid, const wchar_t* desc, const wchar_t*, void* ctx) {
  DeviceInfo info;
  EnumContext* context = static_cast<EnumContext*>(ctx);
  if(context->defaults != (guid == nullptr))
    return TRUE;

  info.output = context->output;
  info.name = XtwWideStringToUtf8(desc);
  info.guid = guid == nullptr? GUID_NULL: *guid;
  context->infos->push_back(info);
  return TRUE;
}

static HRESULT EnumDevices(std::vector<DeviceInfo>& infos, bool defaults) {
  HRESULT hr;
  EnumContext context;
  context.output = false;
  context.infos = &infos;
  context.defaults = defaults;

  XT_VERIFY_COM(DirectSoundCaptureEnumerateW(EnumCallback, &context));
  context.output = true;
  XT_VERIFY_COM(DirectSoundEnumerateW(EnumCallback, &context));
  return S_OK;
}

static HRESULT OpenDevice(const DeviceInfo& info, XtDevice** device) {  
  HRESULT hr;
  CComPtr<IDirectSound> output;
  CComPtr<IDirectSoundCapture> input;

  if(!info.output)
    XT_VERIFY_COM(DirectSoundCaptureCreate8(&info.guid, &input, nullptr));
  else {
    XT_VERIFY_COM(DirectSoundCreate(&info.guid, &output, nullptr));
    XT_VERIFY_COM(output->SetCooperativeLevel(static_cast<HWND>(XtwGetWindow()), DSSCL_PRIORITY));
  }
  *device = new DirectSoundDevice(info.guid, info.name, input, output);
  return S_OK;
}

// ---- service ----

XtFault DirectSoundService::GetFormatFault() const {
  return DSERR_BADFORMAT;
}

XtCapabilities DirectSoundService::GetCapabilities() const {
  return XtCapabilitiesChannelMask;
}

XtFault DirectSoundService::GetDeviceCount(int32_t* count) const {
  HRESULT hr;
  std::vector<DeviceInfo> infos;
  XT_VERIFY_COM(EnumDevices(infos, false));
  *count = static_cast<int32_t>(infos.size());
  return S_OK;
}

XtFault DirectSoundService::OpenDevice(int32_t index, XtDevice** device) const  { 
  HRESULT hr;
  std::vector<DeviceInfo> infos;
  XT_VERIFY_COM(EnumDevices(infos, false));
  if(static_cast<size_t>(index) >= infos.size())
    return DSERR_NODRIVER;
  return ::OpenDevice(infos[index], device);
}

XtFault DirectSoundService::OpenDefaultDevice(XtBool output, XtDevice** device) const  {
  HRESULT hr;
  std::vector<DeviceInfo> infos;
  XT_VERIFY_COM(EnumDevices(infos, true));
  for(size_t i = 0; i < infos.size(); i++) {
    if(infos[i].output == (output != XtFalse))
      return ::OpenDevice(infos[i], device);
  }
  return S_OK;
}

XtCause DirectSoundService::GetFaultCause(XtFault fault) const {
  switch(fault) {
  case DSERR_BADFORMAT: return XtCauseFormat;
  case DSERR_NODRIVER: 
  case DSERR_ALLOCATED: 
  case DSERR_BUFFERLOST: 
  case DSERR_ACCESSDENIED: 
  case DSERR_OTHERAPPHASPRIO: return XtCauseEndpoint;
#ifndef XT_DISABLE_WASAPI
  default: return XtwWasapiGetFaultCause(fault);
#else // XT_DISABLE_WASAPI
  default: return XtCauseUnknown;
#endif // XT_DISABLE_WASAPI
  }
}

const char* DirectSoundService::GetFaultText(XtFault fault) const {
  switch(fault) {
  case DSERR_GENERIC: return "DSERR_GENERIC";
  case DSERR_NODRIVER: return "DSERR_NODRIVER";
  case DSERR_SENDLOOP: return "DSERR_SENDLOOP";
  case DSERR_BADFORMAT: return "DSERR_BADFORMAT";
  case DSERR_ALLOCATED: return "DSERR_ALLOCATED";
  case DSERR_BUFFERLOST: return "DSERR_BUFFERLOST";
  case DSERR_UNSUPPORTED: return "DSERR_UNSUPPORTED";
  case DSERR_INVALIDCALL: return "DSERR_INVALIDCALL";
  case DSERR_NOINTERFACE: return "DSERR_NOINTERFACE";
  case DSERR_OUTOFMEMORY: return "DSERR_OUTOFMEMORY";
  case DSERR_ACCESSDENIED: return "DSERR_ACCESSDENIED";
  case DSERR_INVALIDPARAM: return "DSERR_INVALIDPARAM";
  case DSERR_DS8_REQUIRED: return "DSERR_DS8_REQUIRED";
  case DSERR_NOAGGREGATION: return "DSERR_NOAGGREGATION";
  case DSERR_FXUNAVAILABLE: return "DSERR_FXUNAVAILABLE";
  case DSERR_UNINITIALIZED: return "DSERR_UNINITIALIZED";
  case DSERR_OBJECTNOTFOUND: return "DSERR_OBJECTNOTFOUND";
  case DSERR_BUFFERTOOSMALL: return "DSERR_BUFFERTOOSMALL";
  case DS_NO_VIRTUALIZATION: return "DS_NO_VIRTUALIZATION";
  case DSERR_CONTROLUNAVAIL: return "DSERR_CONTROLUNAVAIL";
  case DSERR_PRIOLEVELNEEDED: return "DSERR_PRIOLEVELNEEDED";
  case DSERR_OTHERAPPHASPRIO: return "DSERR_OTHERAPPHASPRIO";
  case DSERR_BADSENDBUFFERGUID: return "DSERR_BADSENDBUFFERGUID";
  case DSERR_ALREADYINITIALIZED: return "DSERR_ALREADYINITIALIZED";
#ifndef XT_DISABLE_WASAPI
  default: return XtwWasapiGetFaultText(fault);
#else // XT_DISABLE_WASAPI
  default: return "Unknown fault.";
#endif // XT_DISABLE_WASAPI
  }
}

// ---- device ----

XtFault DirectSoundDevice::ShowControlPanel() {
  return S_OK;
}

XtFault DirectSoundDevice::GetName(char* buffer, int32_t* size) const {
  XtiOutputString(this->name.c_str(), buffer, size);
  return S_OK;
}

XtFault DirectSoundDevice::SupportsAccess(XtBool interleaved, XtBool* supports) const {
  *supports = interleaved;
  return S_OK;
}

XtFault DirectSoundDevice::GetBuffer(const XtFormat* format, XtBuffer* buffer) const {
  buffer->min = XtDsMinBufferMs;
  buffer->max = XtDsMaxBufferMs;
  buffer->current = XtDsDefaultBufferMs;
  return S_OK;
}

XtFault DirectSoundDevice::SupportsFormat(const XtFormat* format, XtBool* supports) const {
  *supports = format->channels.inputs > 0 != !input 
           && format->channels.outputs > 0 != !output 
           && format->mix.rate >= 8000 
           && format->mix.rate <= 192000;
  return S_OK;
}

XtFault DirectSoundDevice::GetChannelName(XtBool output, int32_t index, char* buffer, int32_t* size) const {
  XtiOutputString(XtwWfxChannelNames[index], buffer, size);
  return S_OK;
}

XtFault DirectSoundDevice::GetChannelCount(XtBool output, int32_t* count) const {
  *count = (output != XtFalse) == !this->output? 0:  sizeof(XtwWfxChannelNames) / sizeof(const char*);
  return S_OK;
}

XtFault DirectSoundDevice::GetMix(XtBool* valid, XtMix* mix) const {

  UINT count;
  HRESULT hr;
  GUID deviceId;
  XtFormat format;
  CComPtr<IMMDeviceCollection> devices;
  CComPtr<IMMDeviceEnumerator> enumerator;

  GUID thisId = guid;
  if(thisId == GUID_NULL)
    if(!output)
      XT_VERIFY_COM(GetDeviceID(&DSDEVID_DefaultCapture, &thisId));
    else
      XT_VERIFY_COM(GetDeviceID(&DSDEVID_DefaultPlayback, &thisId));
  XT_VERIFY_COM(enumerator.CoCreateInstance(__uuidof(MMDeviceEnumerator)));
  XT_VERIFY_COM(enumerator->EnumAudioEndpoints(eAll, DEVICE_STATE_ACTIVE, &devices));
  XT_VERIFY_COM(devices->GetCount(&count));  

  for(UINT i = 0; i < count; i++) {    
    CComPtr<IMMDevice> device;
    CComPtr<IAudioClient> client;
    CComPtr<IPropertyStore> store;
    CComHeapPtr<WAVEFORMATEX> wfx;
    XtwPropVariant currentIdString;
    XT_VERIFY_COM(devices->Item(i, &device));
    XT_VERIFY_COM(device->OpenPropertyStore(STGM_READ, &store));
    XT_VERIFY_COM(store->GetValue(PKEY_AudioEndpoint_GUID, &currentIdString.pv));
    XT_VERIFY_COM(CLSIDFromString(currentIdString.pv.pwszVal, &deviceId));
    if(deviceId == thisId) {
      XT_VERIFY_COM(device->Activate(__uuidof(IAudioClient), CLSCTX_ALL, nullptr, reinterpret_cast<void**>(&client)));
      XT_VERIFY_COM(client->GetMixFormat(&wfx));
      if(!XtwWfxToFormat(*wfx, !input, format))
        return DSERR_BADFORMAT;
      *valid = XtTrue;
      mix->rate = format.mix.rate;
      mix->sample = format.mix.sample;
      return S_OK;
    }
  }
  return S_OK;
}

XtFault DirectSoundDevice::OpenStream(const XtFormat* format, XtBool interleaved, double bufferSize, 
                                 bool secondary, XtStreamCallback callback, void* user, XtStream** stream) {

  HRESULT hr;
  int32_t frameSize;
  int32_t bufferFrames;
  WAVEFORMATEXTENSIBLE wfx;
  DSBUFFERDESC renderDesc = { 0 };
  DSCBUFFERDESC captureDesc = { 0 };
  CComPtr<IDirectSound> newOutput;
  CComPtr<IDirectSoundBuffer> render;
  CComPtr<IDirectSoundCapture> newInput;
  CComPtr<IDirectSoundCaptureBuffer> capture;

  XT_ASSERT(XtwFormatToWfx(*format, wfx));
  if(bufferSize < XtDsMinBufferMs)
    bufferSize = XtDsMinBufferMs;
  if(bufferSize > XtDsMaxBufferMs)
    bufferSize = XtDsMaxBufferMs;
  bufferFrames = static_cast<int32_t>(bufferSize / 1000.0 * format->mix.rate);
  frameSize = (format->channels.inputs + format->channels.outputs) * XtiGetSampleSize(format->mix.sample);

  if(input) {
    captureDesc.dwSize = sizeof(DSCBUFFERDESC);
    captureDesc.dwBufferBytes = bufferFrames * frameSize;
    captureDesc.lpwfxFormat = reinterpret_cast<WAVEFORMATEX*>(&wfx);
    XT_VERIFY_COM(DirectSoundCaptureCreate8(&guid, &newInput, nullptr));
    XT_VERIFY_COM(newInput->CreateCaptureBuffer(&captureDesc, &capture, nullptr));
  } else {
    renderDesc.dwFlags = DSBCAPS_GLOBALFOCUS | DSBCAPS_TRUEPLAYPOSITION;
    renderDesc.dwSize = sizeof(DSBUFFERDESC);
    renderDesc.dwBufferBytes = bufferFrames * frameSize;
    renderDesc.lpwfxFormat = reinterpret_cast<WAVEFORMATEX*>(&wfx);
    XT_VERIFY_COM(DirectSoundCreate(&guid, &newOutput, nullptr));
    XT_VERIFY_COM(newOutput->SetCooperativeLevel(static_cast<HWND>(XtwGetWindow()), DSSCL_PRIORITY));
    XT_VERIFY_COM(newOutput->CreateSoundBuffer(&renderDesc, &render, nullptr));
  }

  *stream = new DirectSoundStream(secondary, newInput, newOutput, capture, render, bufferFrames, frameSize);
  return S_OK;
}

// ---- stream ----

XtFault DirectSoundStream::GetFrames(int32_t* frames) const {
  *frames = bufferFrames;
  return S_OK;
}

XtFault DirectSoundStream::GetLatency(XtLatency* latency) const {
  return S_OK;
}

void DirectSoundStream::StopStream() {
  if(capture)
    XT_ASSERT(SUCCEEDED(capture->Stop()));
  else
    XT_ASSERT(SUCCEEDED(render->Stop()));
  if(!secondary) {
    XT_ASSERT(CancelWaitableTimer(timer.timer));
    XT_ASSERT(timeEndPeriod(GetTimerPeriod(bufferFrames, format.mix.rate) / 2) == TIMERR_NOERROR);
    XT_ASSERT(SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_NORMAL));
  }
  xtBytesProcessed = 0;
  dsBytesProcessed = 0;
  previousDsPosition = 0;
}

void DirectSoundStream::StartStream() {
  LARGE_INTEGER due;
  due.QuadPart = -1;
  UINT timerPeriod = GetTimerPeriod(bufferFrames, format.mix.rate);
  long lTimerPeriod = timerPeriod;
  if(!secondary) {
    XT_ASSERT(SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL));
    XT_ASSERT(timeBeginPeriod(timerPeriod / 2) == TIMERR_NOERROR);
    XT_ASSERT(SetWaitableTimer(timer.timer, &due, lTimerPeriod, nullptr, nullptr, TRUE));
  }
  if(capture)
    XT_ASSERT(SUCCEEDED(capture->Start(DSCBSTART_LOOPING)));
  else
    XT_ASSERT(SUCCEEDED(render->Play(0, 0, DSBPLAY_LOOPING)));
}

void DirectSoundStream::ProcessBuffer(bool prefill) {
  
  void* audio1;
  void* audio2;
  int32_t gap, available, dsProcessed;
  DWORD bufferBytes = static_cast<DWORD>(buffer.size());
  DWORD size1, size2, read, write, lockPosition, waitResult;
  DWORD bufferMillis = static_cast<DWORD>(bufferFrames * 1000.0 / format.mix.rate);

  if(!prefill && !secondary) {
    waitResult = WaitForSingleObject(timer.timer, bufferMillis);
    if(waitResult == WAIT_TIMEOUT)
      return;
    XT_ASSERT(waitResult == WAIT_OBJECT_0);
  }
  
  if(prefill && render) {
    if(!XT_VERIFY_STREAM_CALLBACK(render->Lock(0, bufferBytes, &audio1, &size1, &audio2, &size2, 0)))
      return;
    if(size2 == 0) {
      ProcessCallback(nullptr, audio1, bufferFrames, 0.0, 0, XtFalse, DS_OK);
    } else {
      ProcessCallback(nullptr, &buffer[0], bufferFrames, 0.0, 0, XtFalse, DS_OK);
      SplitBufferParts(buffer, audio1, size1, audio2, size2);
    }
    if(!XT_VERIFY_STREAM_CALLBACK(render->Unlock(audio1, size1, audio2, size2)))
      return;
    xtBytesProcessed += bufferBytes;
    return;
  }

  if(capture && !prefill) {
    if(!XT_VERIFY_STREAM_CALLBACK(capture->GetCurrentPosition(&write, &read)))
      return;
    gap = WrapAround(write - read, bufferBytes);
    dsProcessed = WrapAround(write - previousDsPosition, bufferBytes);
    dsBytesProcessed += dsProcessed;
    lockPosition = xtBytesProcessed % bufferBytes;
    available = static_cast<int32_t>(dsBytesProcessed - xtBytesProcessed - gap);
    previousDsPosition = write;
    if(available <= 0)
      return;
    if(InsideSafetyGap(read, write, lockPosition)) {
      XT_VERIFY_STREAM_CALLBACK(DSERR_BUFFERLOST);
      return;
    }
    if(!XT_VERIFY_STREAM_CALLBACK(capture->Lock(lockPosition, available, &audio1, &size1, &audio2, &size2, 0)))
      return;
    if(size2 == 0) {
      ProcessCallback(audio1, nullptr, available / frameSize, 0.0, 0, XtFalse, S_OK);
      if(!XT_VERIFY_STREAM_CALLBACK(capture->Unlock(audio1, size1, audio2, size2)))
        return;
    } else {
      CombineBufferParts(buffer, audio1, size1, audio2, size2);
      if(!XT_VERIFY_STREAM_CALLBACK(capture->Unlock(audio1, size1, audio2, size2)))
        return;
      ProcessCallback(&buffer[0], nullptr, available / frameSize, 0.0, 0, XtFalse, S_OK);
    }
    xtBytesProcessed += available;
  }

  if(render && !prefill) {
    if(!XT_VERIFY_STREAM_CALLBACK(render->GetCurrentPosition(&read, &write)))
      return;
    gap = WrapAround(write - read, bufferBytes);
    dsProcessed = WrapAround(read - previousDsPosition, bufferBytes);
    dsBytesProcessed += dsProcessed;
    lockPosition = xtBytesProcessed % bufferBytes;
    available = static_cast<int32_t>(bufferBytes - gap - (xtBytesProcessed - dsBytesProcessed));
    previousDsPosition = read;
    if(available <= 0)
      return;
    if(InsideSafetyGap(read, write, lockPosition)) {
      XT_VERIFY_STREAM_CALLBACK(DSERR_BUFFERLOST);
      return;
    }
    if(!XT_VERIFY_STREAM_CALLBACK(render->Lock(lockPosition, available, &audio1, &size1, &audio2, &size2, 0)))
      return;
    if(size2 == 0) {
      ProcessCallback(nullptr, audio1, available / frameSize, 0.0, 0, XtFalse, DS_OK);
    } else {
      ProcessCallback(nullptr, &buffer[0], available / frameSize, 0.0, 0, XtFalse, DS_OK);
      SplitBufferParts(buffer, audio1, size1, audio2, size2);
    }
    if(!XT_VERIFY_STREAM_CALLBACK(render->Unlock(audio1, size1, audio2, size2)))
      return;
    xtBytesProcessed += available;
  }
}

#endif // XT_DISABLE_DIRECT_SOUND
#endif // _WIN32