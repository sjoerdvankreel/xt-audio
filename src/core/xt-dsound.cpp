#ifdef _WIN32
#include "xt-win32.hpp"
#include <dsound.h>
#include <mmdeviceapi.h>
#include <audioclient.h>
#include <vector>
#include <memory>

/* Copyright (C) 2015-2016 Sjoerd van Kreel.
 *
 * This file is part of XT-Audio.
 *
 * XT-Audio is free software: you can redistribute it and/or modify it under the 
 * terms of the GNU Lesser General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later version.
 *
 * XT-Audio is distributed in the hope that it will be useful, but WITHOUT ANY 
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with XT-Audio. If not, see<http://www.gnu.org/licenses/>.
 */

// ---- local ----

static const int XtDsWakeUpsPerBuffer = 8;
static const double XtDsMinBufferMs = 100.0;
static const double XtDsMaxBufferMs = 5000.0;
static const double XtDsDefaultBufferMs = 500.0;

// ---- forward ----

XT_DECLARE_SERVICE(DSound);

struct DSoundDevice: public XtDevice {
  const GUID guid;
  const std::string name;
  const CComPtr<IDirectSound> output;
  const CComPtr<IDirectSoundCapture> input;
  XT_IMPLEMENT_DEVICE(DSound);

  DSoundDevice(
    const GUID& g, const std::string& n, 
    CComPtr<IDirectSoundCapture> i, CComPtr<IDirectSound> o):
  XtDevice(), guid(g), name(n), output(o), input(i) {}
};

struct DSoundStream: public XtwWin32Stream {
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
  XT_IMPLEMENT_STREAM(DSound);

  ~DSoundStream() { Stop(); }
  DSoundStream(bool secondary,
    CComPtr<IDirectSoundCapture> input, CComPtr<IDirectSound> output,
    CComPtr<IDirectSoundCaptureBuffer> capture, CComPtr<IDirectSoundBuffer> render, 
    int32_t bufferFrames, int32_t frameSize):
  XtwWin32Stream(secondary), frameSize(frameSize),
  buffer(static_cast<size_t>(bufferFrames * frameSize), '\0'),
  xtBytesProcessed(0), dsBytesProcessed(0),
  previousDsPosition(0), bufferFrames(bufferFrames), timer(),
  output(output), input(input), render(render), capture(capture) {}

  void StopStream();
  void StartStream();
  void ProcessBuffer(bool prefill);  
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
  *device = new DSoundDevice(info.guid, info.name, input, output);
  return S_OK;
}

// ---- service ----

const char* DSoundService::GetName() const {
  return "DirectSound";
}

XtFault DSoundService::GetFormatFault() const {
  return DSERR_BADFORMAT;
}

XtCapabilities DSoundService::GetCapabilities() const {
  return XtCapabilitiesChannelMask;
}

XtFault DSoundService::GetDeviceCount(int32_t* count) const {
  HRESULT hr;
  std::vector<DeviceInfo> infos;
  XT_VERIFY_COM(EnumDevices(infos, false));
  *count = static_cast<int32_t>(infos.size());
  return S_OK;
}

XtFault DSoundService::OpenDevice(int32_t index, XtDevice** device) const  { 
  HRESULT hr;
  std::vector<DeviceInfo> infos;
  XT_VERIFY_COM(EnumDevices(infos, false));
  if(static_cast<size_t>(index) >= infos.size())
    return DSERR_NODRIVER;
  return ::OpenDevice(infos[index], device);
}

XtFault DSoundService::OpenDefaultDevice(XtBool output, XtDevice** device) const  {
  HRESULT hr;
  std::vector<DeviceInfo> infos;
  XT_VERIFY_COM(EnumDevices(infos, true));
  for(size_t i = 0; i < infos.size(); i++) {
    if(infos[i].output == (output != XtFalse))
      return ::OpenDevice(infos[i], device);
  }
  return S_OK;
}

XtCause DSoundService::GetFaultCause(XtFault fault) const {
  switch(fault) {
  case DSERR_BADFORMAT: return XtCauseFormat;
  case DSERR_NODRIVER: 
  case DSERR_ALLOCATED: 
  case DSERR_BUFFERLOST: 
  case DSERR_ACCESSDENIED: 
  case DSERR_OTHERAPPHASPRIO: return XtCauseEndpoint;
  default: return XtwWasapiGetFaultCause(fault);
  }
}

const char* DSoundService::GetFaultText(XtFault fault) const {
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
  default: return XtwWasapiGetFaultText(fault);
  }
}

// ---- device ----

XtFault DSoundDevice::ShowControlPanel() {
  return S_OK;
}

XtFault DSoundDevice::GetName(char** name) const {
  *name = _strdup(this->name.c_str());
  return S_OK;
}

XtFault DSoundDevice::SupportsAccess(XtBool interleaved, XtBool* supports) const {
  *supports = interleaved;
  return S_OK;
}

XtFault DSoundDevice::GetBuffer(const XtFormat* format, XtBuffer* buffer) const {
  buffer->min = XtDsMinBufferMs;
  buffer->max = XtDsMaxBufferMs;
  buffer->current = XtDsDefaultBufferMs;
  return S_OK;
}

XtFault DSoundDevice::SupportsFormat(const XtFormat* format, XtBool* supports) const {
  *supports = format->inputs > 0 != !input 
           && format->outputs > 0 != !output 
           && format->mix.rate >= 8000 
           && format->mix.rate <= 192000;
  return S_OK;
}

XtFault DSoundDevice::GetChannelName(XtBool output, int32_t index, char** name) const {
  *name = _strdup(XtwWfxChannelNames[index]);
  return S_OK;
}

XtFault DSoundDevice::GetChannelCount(XtBool output, int32_t* count) const {
  *count = (output != XtFalse) == !this->output? 0:  sizeof(XtwWfxChannelNames) / sizeof(const char*);
  return S_OK;
}

XtFault DSoundDevice::GetMix(XtMix** mix) const {

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
      *mix = static_cast<XtMix*>(malloc(sizeof(XtMix)));
      (*mix)->rate = format.mix.rate;
      (*mix)->sample = format.mix.sample;
      return S_OK;
    }
  }
  return S_OK;
}

XtFault DSoundDevice::OpenStream(const XtFormat* format, XtBool interleaved, double bufferSize, 
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
  frameSize = (format->inputs + format->outputs) * XtiGetSampleSize(format->mix.sample);

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

  *stream = new DSoundStream(secondary, newInput, newOutput, capture, render, bufferFrames, frameSize);
  return S_OK;
}

// ---- stream ----

XtFault DSoundStream::GetFrames(int32_t* frames) const {
  *frames = bufferFrames;
  return S_OK;
}

XtFault DSoundStream::GetLatency(XtLatency* latency) const {
  return S_OK;
}

void DSoundStream::StopStream() {
  if(capture)
    XT_ASSERT(SUCCEEDED(capture->Stop()));
  else
    XT_ASSERT(SUCCEEDED(render->Stop()));
  XT_ASSERT(CancelWaitableTimer(timer.timer));
  XT_ASSERT(timeEndPeriod(GetTimerPeriod(bufferFrames, format.mix.rate) / 2) == TIMERR_NOERROR);
  XT_ASSERT(SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_NORMAL));
  xtBytesProcessed = 0;
  dsBytesProcessed = 0;
  previousDsPosition = 0;
}

void DSoundStream::StartStream() {
  LARGE_INTEGER due;
  due.QuadPart = -1;
  UINT timerPeriod = GetTimerPeriod(bufferFrames, format.mix.rate);
  long lTimerPeriod = timerPeriod;
  XT_ASSERT(SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL));
  XT_ASSERT(timeBeginPeriod(timerPeriod / 2) == TIMERR_NOERROR);
  XT_ASSERT(SetWaitableTimer(timer.timer, &due, lTimerPeriod, nullptr, nullptr, TRUE));
  if(capture)
    XT_ASSERT(SUCCEEDED(capture->Start(DSCBSTART_LOOPING)));
  else
    XT_ASSERT(SUCCEEDED(render->Play(0, 0, DSBPLAY_LOOPING)));
}

void DSoundStream::ProcessBuffer(bool prefill) {
  
  void* audio1;
  void* audio2;
  int32_t gap, available, dsProcessed;
  DWORD bufferBytes = static_cast<DWORD>(buffer.size());
  DWORD size1, size2, read, write, lockPosition, waitResult;
  DWORD bufferMillis = static_cast<DWORD>(bufferFrames * 1000.0 / format.mix.rate);

  if(!prefill) {
    waitResult = WaitForSingleObject(timer.timer, bufferMillis);
    if(waitResult == WAIT_TIMEOUT)
      return;
    XT_ASSERT(waitResult == WAIT_OBJECT_0);
  }
  else if(render) {
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

  if(capture) {
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

  if(render) {
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

#endif // _WIN32
