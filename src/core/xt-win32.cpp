#ifdef _WIN32
#include "xt-win32.hpp"

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

static HWND XtwWindow = nullptr;
static DWORD XtwMainThreadId = 0;
static bool XtwOwnWindow = false;

static XtStreamState ReadWin32StreamState(
  XtwWin32Stream* stream) {

  XtStreamState result;
  EnterCriticalSection(&stream->lock.cs);
  result = stream->state;
  LeaveCriticalSection(&stream->lock.cs);
  return result;
}

static void ReceiveWin32StreamControl(
  XtwWin32Stream* stream, XtStreamState state) {

  EnterCriticalSection(&stream->lock.cs);
  stream->state = state;
  XT_ASSERT(SetEvent(stream->respondEvent.event));
  LeaveCriticalSection(&stream->lock.cs);
}

static void SendWin32StreamControl(
  XtwWin32Stream* stream, XtStreamState from, XtStreamState to) {

  EnterCriticalSection(&stream->lock.cs);
  if(stream->state == to) {
    LeaveCriticalSection(&stream->lock.cs);
    return;
  }
  stream->state = from;
  XT_ASSERT(SetEvent(stream->controlEvent.event));
  LeaveCriticalSection(&stream->lock.cs);
  while(ReadWin32StreamState(stream) != to)
    XT_ASSERT(WaitForSingleObject(stream->respondEvent.event, XT_WAIT_TIMEOUT_MS) == WAIT_OBJECT_0);
}

static DWORD WINAPI Win32StreamCallback(void* user) {
  XtStreamState state;
  auto stream = static_cast<XtwWin32Stream*>(user);

  XT_ASSERT(SUCCEEDED(CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED)));
  while((state = ReadWin32StreamState(stream)) != XtStreamStateClosed) {
    switch(state) {
    case XtStreamStateStarted:
      stream->ProcessBuffer(false);
      break;
    case XtStreamStateClosing:
      ReceiveWin32StreamControl(stream, XtStreamStateClosed);
      CoUninitialize();
      return S_OK;
    case XtStreamStateStopping:
      stream->StopStream();
      ReceiveWin32StreamControl(stream, XtStreamStateStopped);
      break;
    case XtStreamStateStarting:
      stream->ProcessBuffer(true);
      stream->StartStream();
      ReceiveWin32StreamControl(stream, XtStreamStateStarted);
      break;
    case XtStreamStateStopped:
      XT_ASSERT(WaitForSingleObject(stream->controlEvent.event, INFINITE) == WAIT_OBJECT_0);
      break;
    default:
      XT_FAIL("Unexpected stream state.");
      break;
    }
  }
  XT_FAIL("End of stream callback reached.");
  return S_OK;
}

// ---- win32 ----

void* XtwGetWindow() {
  return XtwWindow; 
}

const char* XtwWfxChannelNames[18] = {
  "Front Left", "Front Right", "Front Center",
  "Low Frequency", "Back Left", "Back Right",
  "Front Left Of Center", "Front Right Of Center", "Back Center", 
  "Side Left", "Side Right", "Top Center",
  "Top Front Left", "Top Front Center", "Top Front Right",
  "Top Back Left", "Top Back Center", "Top Back Right"
};

// ---- api ----

XtBool XT_CALL XtAudioIsWin32(void) {
  return XtTrue;
}

int32_t XT_CALL XtAudioGetServiceCount(void) { 
  return 3;
}

const XtService* XT_CALL XtAudioGetServiceBySystem(XtSystem system) {
  switch(system) {
  case XtSystemAsio: return XtiServiceAsio;
  case XtSystemDSound: return XtiServiceDSound;
  case XtSystemWasapi: return XtiServiceWasapi;
  default: return XT_FAIL("Unknown system."), nullptr;
  }
}

// ---- internal ----

XtSystem XtiSetupToSystem(XtSetup setup) {
  switch(setup) {
  case XtSetupProAudio: return XtSystemAsio;
  case XtSetupSystemAudio: return XtSystemWasapi;
  case XtSetupConsumerAudio: return XtSystemDSound;
  default: return XT_FAIL("Unknown setup."), XtSystemDSound;
  }
}

XtSystem XtiIndexToSystem(int32_t index) {
  switch(index) {
  case 0: return XtSystemDSound;
  case 1: return XtSystemWasapi;
  case 2: return XtSystemAsio;
  default: return XT_FAIL("Unknown index."), XtSystemDSound;
  }
}

void XtiTerminatePlatform() { 
  XT_ASSERT(!XtwOwnWindow || DestroyWindow(XtwWindow));
  CoUninitialize(); 
  XtwWindow = nullptr;
  XtwMainThreadId = 0;
  XtwOwnWindow = false;
}

bool XtiCalledOnMainThread() {
  DWORD currentThreadId;
  XT_ASSERT((currentThreadId = GetThreadId(GetCurrentThread())) != 0);
  return currentThreadId == XtwMainThreadId;
}

int32_t XtiLockIncr(volatile int32_t* dest) {
  return InterlockedIncrement(reinterpret_cast<volatile long*>(dest));
}

int32_t XtiLockDecr(volatile int32_t* dest) {
  return InterlockedDecrement(reinterpret_cast<volatile long*>(dest));
}

int32_t XtiCas(volatile int32_t* dest, int32_t exch, int32_t comp) {
  return InterlockedCompareExchange(reinterpret_cast<volatile long*>(dest), exch, comp);
}

void XtiInitPlatform(void* wnd) {
  XT_ASSERT(XtwMainThreadId == 0);
  XT_ASSERT((XtwMainThreadId = GetThreadId(GetCurrentThread())) != 0);
  XT_ASSERT(SUCCEEDED(CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED)));
  if((XtwWindow = static_cast<HWND>(wnd)) == nullptr) {
    XT_ASSERT(XtwWindow = CreateWindow("STATIC", 0, 0, 0, 0, 0, 0, HWND_MESSAGE, 0, 0, 0));
    XtwOwnWindow = true;
  }
}

// ---- win32 ----

XtwWin32Stream::XtwWin32Stream(bool secondary):
XtManagedStream(secondary),
state(XtStreamStateStopped), 
lock(),
respondEvent(),
controlEvent() {
  if(!secondary) {
    HANDLE thread = CreateThread(nullptr, 0, &Win32StreamCallback, this, 0, nullptr);
    XT_ASSERT(thread != nullptr);
    CloseHandle(thread);
  }
}

XtwWin32Stream::~XtwWin32Stream() {
  if(!secondary)
    SendWin32StreamControl(this, XtStreamStateClosing, XtStreamStateClosed);
}

XtFault XtwWin32Stream::Start() {
  if(!secondary)
    SendWin32StreamControl(this, XtStreamStateStarting, XtStreamStateStarted);
  else {
    ProcessBuffer(true);
    StartStream();
  }
  return S_OK;
}

XtFault XtwWin32Stream::Stop() {
  if(secondary)
    StopStream();
  else
    SendWin32StreamControl(this, XtStreamStateStopping, XtStreamStateStopped);
  return S_OK;
}

void XtwWin32Stream::RequestStop() {
  StopStream();
  if(!secondary) {
    EnterCriticalSection(&lock.cs);
    state = XtStreamStateStopped;
    XT_ASSERT(SetEvent(respondEvent.event));
    LeaveCriticalSection(&lock.cs);
  }
}

bool XtwWin32Stream::VerifyStreamCallback(HRESULT hr, const char* file, int line, const char* func, const char* expr) {
  if(SUCCEEDED(hr))
    return true;
  RequestStop();
  XtiTrace(XtLevelError, file, line, func, expr);
  ProcessCallback(nullptr, nullptr, 0, 0.0, 0, XtFalse, XtiCreateError(XtStreamGetSystem(this), hr));
  return false;
}

std::string XtwWideStringToUtf8(const wchar_t* wide) {
  int count;
  XT_ASSERT((count = WideCharToMultiByte(CP_UTF8, 0, wide, -1, nullptr, 0, nullptr, 0)) > 0);
  std::string result(count - 1, '\0');
  XT_ASSERT(WideCharToMultiByte(CP_UTF8, 0, wide, -1, &result[0], count, nullptr, 0) > 0);
  return result;
}

bool XtwWfxToFormat(const WAVEFORMATEX& wfx, XtBool output, XtFormat& format) {  

  memset(&format, 0, sizeof(XtFormat));
  const WAVEFORMATEXTENSIBLE* wfxe = nullptr;
  if(wfx.wFormatTag == WAVE_FORMAT_EXTENSIBLE)
    wfxe = reinterpret_cast<const WAVEFORMATEXTENSIBLE*>(&wfx);
  
  format.mix.rate = wfx.nSamplesPerSec;
  format.inputs = output? 0: wfx.nChannels;
  format.outputs = output? wfx.nChannels: 0;
  format.inMask = output || wfxe == nullptr? 0: wfxe->dwChannelMask;
  format.outMask = !output || wfxe == nullptr? 0: wfxe->dwChannelMask;

  if(wfxe != nullptr && wfx.wBitsPerSample != wfxe->Samples.wValidBitsPerSample)
    return false;
  if(wfxe != nullptr && wfxe->SubFormat == KSDATAFORMAT_SUBTYPE_IEEE_FLOAT)
    format.mix.sample = XtSampleFloat32;
  else
    switch(wfx.wBitsPerSample) {
    case 8: format.mix.sample = XtSampleUInt8; break;
    case 16: format.mix.sample = XtSampleInt16; break;
    case 24: format.mix.sample = XtSampleInt24; break;
    case 32: format.mix.sample = XtSampleInt32; break;
    default: return false;
    }
  return true;
}

bool XtwFormatToWfx(const XtFormat& format, WAVEFORMATEXTENSIBLE& wfx) {  

  XtAttributes attributes;     
  memset(&wfx, 0, sizeof(WAVEFORMATEXTENSIBLE));
  if(format.inputs > 0 && format.outputs > 0)
    return false;

  XtAudioGetSampleAttributes(format.mix.sample, &attributes);
  wfx.Format.cbSize = 22;
  wfx.Format.nSamplesPerSec = format.mix.rate;
  wfx.Format.wFormatTag = WAVE_FORMAT_EXTENSIBLE;
  wfx.Format.nChannels = format.inputs + format.outputs;
  wfx.Format.wBitsPerSample = XtiGetSampleSize(format.mix.sample) * 8;
  wfx.Format.nBlockAlign = wfx.Format.wBitsPerSample / 8 * wfx.Format.nChannels;
  wfx.Format.nAvgBytesPerSec = wfx.Format.nBlockAlign * format.mix.rate;
  wfx.SubFormat = attributes.isFloat? KSDATAFORMAT_SUBTYPE_IEEE_FLOAT: KSDATAFORMAT_SUBTYPE_PCM;
  wfx.Samples.wValidBitsPerSample = wfx.Format.wBitsPerSample;
  wfx.dwChannelMask = static_cast<DWORD>(format.inputs? format.inMask: format.outMask);
  if(wfx.dwChannelMask == 0)
    wfx.dwChannelMask = (1U << wfx.Format.nChannels) - 1;
  return true;
}

#endif // _WIN32