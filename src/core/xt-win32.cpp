#ifdef _WIN32
#include "xt-win32.hpp"

// ---- local ----

static HWND XtwWindow = nullptr;
static DWORD XtwMainThreadId = 0;
static bool XtwOwnWindow = false;

extern const XtService* XtiServiceAsio;
extern const XtService* XtiServiceDSound;
extern const XtService* XtiServiceWasapi;

const XtService* const XtiServices[] =
{
#ifndef XT_DISABLE_DSOUND
  XtiServiceDSound,
#endif // XT_DISABLE_DSOUND
#ifndef XT_DISABLE_WASAPI
  XtiServiceWasapi,
#endif // XT_DISABLE_WASAPI
#ifndef XT_DISABLE_ASIO
  XtiServiceAsio,
#endif // XT_DISABLE_ASIO
  nullptr,
};

static XtBlockingStreamState ReadWin32BlockingStreamState(
  XtwWin32BlockingStream* stream) {

  XtBlockingStreamState result;
  EnterCriticalSection(&stream->lock.cs);
  result = stream->state;
  LeaveCriticalSection(&stream->lock.cs);
  return result;
}

static void ReceiveWin32BlockingStreamControl(
  XtwWin32BlockingStream* stream, XtBlockingStreamState state) {

  EnterCriticalSection(&stream->lock.cs);
  stream->state = state;
  XT_ASSERT(SetEvent(stream->respondEvent.event));
  LeaveCriticalSection(&stream->lock.cs);
}

static void SendWin32BlockingStreamControl(
  XtwWin32BlockingStream* stream, XtBlockingStreamState from, XtBlockingStreamState to) {

  EnterCriticalSection(&stream->lock.cs);
  if(stream->state == to) {
    LeaveCriticalSection(&stream->lock.cs);
    return;
  }
  stream->state = from;
  XT_ASSERT(SetEvent(stream->controlEvent.event));
  LeaveCriticalSection(&stream->lock.cs);
  while(ReadWin32BlockingStreamState(stream) != to)
    XT_ASSERT(WaitForSingleObject(stream->respondEvent.event, XT_WAIT_TIMEOUT_MS) == WAIT_OBJECT_0);
}

static DWORD WINAPI Win32BlockingStreamCallback(void* user) {
  XtBlockingStreamState state;
  auto stream = static_cast<XtwWin32BlockingStream*>(user);

  XT_ASSERT(SUCCEEDED(CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED)));
  while((state = ReadWin32BlockingStreamState(stream)) != XtBlockingStreamState::Closed) {
    switch(state) {
    case XtBlockingStreamState::Started:
      stream->ProcessBuffer(false);
      break;
    case XtBlockingStreamState::Closing:
      ReceiveWin32BlockingStreamControl(stream, XtBlockingStreamState::Closed);
      CoUninitialize();
      return S_OK;
    case XtBlockingStreamState::Stopping:
      stream->StopStream();
      ReceiveWin32BlockingStreamControl(stream, XtBlockingStreamState::Stopped);
      break;
    case XtBlockingStreamState::Starting:
      stream->ProcessBuffer(true);
      stream->StartStream();
      ReceiveWin32BlockingStreamControl(stream, XtBlockingStreamState::Started);
      break;
    case XtBlockingStreamState::Stopped:
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

int32_t XT_CALL XtAudioGetServiceCount(void) { 
  return sizeof(XtiServices) / sizeof(XtiServices[0]) - 1;
}

const XtService* XT_CALL XtAudioGetServiceByIndex(int32_t index) {
  XT_ASSERT(0 <= index && index < XtAudioGetServiceCount());
  return XtiServices[index];
}

const XtService* XT_CALL XtAudioGetServiceBySystem(XtSystem system) {
  XT_ASSERT(XtSystemAlsa <= system && system <= XtSystemWasapi);
  switch(system) {
  case XtSystemAsio: return XtiServiceAsio;
  case XtSystemDSound: return XtiServiceDSound;
  case XtSystemWasapi: return XtiServiceWasapi;
  case XtSystemAlsa:
  case XtSystemJack:
  case XtSystemPulse: return nullptr;
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
  XT_TRACE(XtLevelInfo, "Built with ASIO: %d.", XtAudioGetServiceBySystem(XtSystemAsio) != nullptr);
  XT_TRACE(XtLevelInfo, "Built with WASAPI: %d.", XtAudioGetServiceBySystem(XtSystemWasapi) != nullptr);
  XT_TRACE(XtLevelInfo, "Built with DirectSound: %d.", XtAudioGetServiceBySystem(XtSystemDSound) != nullptr);
}

// ---- win32 ----

XtwWin32BlockingStream::XtwWin32BlockingStream(bool secondary):
XtBlockingStream(secondary),
state(XtBlockingStreamState::Stopped), 
lock(),
respondEvent(),
controlEvent() {
  if(!secondary) {
    HANDLE thread = CreateThread(nullptr, 0, &Win32BlockingStreamCallback, this, 0, nullptr);
    XT_ASSERT(thread != nullptr);
    CloseHandle(thread);
  }
}

XtwWin32BlockingStream::~XtwWin32BlockingStream() {
  if(!secondary)
    SendWin32BlockingStreamControl(this, XtBlockingStreamState::Closing, XtBlockingStreamState::Closed);
}

XtFault XtwWin32BlockingStream::Start() {
  if(!secondary)
    SendWin32BlockingStreamControl(this, XtBlockingStreamState::Starting, XtBlockingStreamState::Started);
  else {
    ProcessBuffer(true);
    StartStream();
  }
  return S_OK;
}

XtFault XtwWin32BlockingStream::Stop() {
  if(secondary)
    StopStream();
  else
    SendWin32BlockingStreamControl(this, XtBlockingStreamState::Stopping, XtBlockingStreamState::Stopped);
  return S_OK;
}

void XtwWin32BlockingStream::RequestStop() {
  StopStream();
  if(!secondary) {
    EnterCriticalSection(&lock.cs);
    state = XtBlockingStreamState::Stopped;
    XT_ASSERT(SetEvent(respondEvent.event));
    LeaveCriticalSection(&lock.cs);
  }
}

bool XtwWin32BlockingStream::VerifyStreamCallback(HRESULT hr, const char* file, int line, const char* func, const char* expr) {
  if(SUCCEEDED(hr))
    return true;
  RequestStop();
  XtiTrace(XtLevelError, file, line, func, expr);
  ProcessCallback(nullptr, nullptr, 0, 0.0, 0, XtFalse, XtiCreateError(GetSystem(), hr));
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
  format.channels.inputs = output? 0: wfx.nChannels;
  format.channels.outputs = output? wfx.nChannels: 0;
  format.channels.inMask = output || wfxe == nullptr? 0: wfxe->dwChannelMask;
  format.channels.outMask = !output || wfxe == nullptr? 0: wfxe->dwChannelMask;

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

  memset(&wfx, 0, sizeof(WAVEFORMATEXTENSIBLE));
  if(format.channels.inputs > 0 && format.channels.outputs > 0)
    return false;

  auto attributes = XtAudioGetSampleAttributes(format.mix.sample);
  wfx.Format.cbSize = 22;
  wfx.Format.nSamplesPerSec = format.mix.rate;
  wfx.Format.wFormatTag = WAVE_FORMAT_EXTENSIBLE;
  wfx.Format.nChannels = format.channels.inputs + format.channels.outputs;
  wfx.Format.wBitsPerSample = XtiGetSampleSize(format.mix.sample) * 8;
  wfx.Format.nBlockAlign = wfx.Format.wBitsPerSample / 8 * wfx.Format.nChannels;
  wfx.Format.nAvgBytesPerSec = wfx.Format.nBlockAlign * format.mix.rate;
  wfx.SubFormat = attributes.isFloat? KSDATAFORMAT_SUBTYPE_IEEE_FLOAT: KSDATAFORMAT_SUBTYPE_PCM;
  wfx.Samples.wValidBitsPerSample = wfx.Format.wBitsPerSample;
  wfx.dwChannelMask = static_cast<DWORD>(format.channels.inputs? format.channels.inMask: format.channels.outMask);
  if(wfx.dwChannelMask == 0)
    wfx.dwChannelMask = (1U << wfx.Format.nChannels) - 1;
  return true;
}

#endif // _WIN32