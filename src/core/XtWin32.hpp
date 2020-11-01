#ifndef _XT_WIN32_HPP
#define _XT_WIN32_HPP
#ifdef _WIN32

#ifndef NOMINMAX
#define NOMINMAX 1
#endif // NOMINMAX
#include "XtPrivate.hpp"
#include <windows.h>
#include <atlbase.h>
#include <mmsystem.h>
#include <mmreg.h>
#include <ks.h>
#include <ksmedia.h>
#include <string>

// ---- win32 ----

#define XT_VERIFY_COM(e)                   \
do {                                       \
  if(FAILED(hr = (e)))                     \
    return XT_TRACE(#e), hr; \
} while(0)

extern const char* XtwWfxChannelNames[18];

void* XtwGetWindow();
XtCause XtwWasapiGetFaultCause(XtFault fault);
const char* XtwWasapiGetFaultText(XtFault fault);
std::string XtwWideStringToUtf8(const wchar_t* wide);
bool XtwFormatToWfx(const XtFormat& format, WAVEFORMATEXTENSIBLE& wfx);
bool XtwWfxToFormat(const WAVEFORMATEX& wfx, XtBool output, XtFormat& format);

struct XtwPropVariant {
  PROPVARIANT pv;
  XtwPropVariant(const XtwPropVariant&) = delete;
  XtwPropVariant& operator=(const XtwPropVariant&) = delete;
  ~XtwPropVariant() { PropVariantClear(&pv); }
  XtwPropVariant(): pv() { PropVariantInit(&pv); }
};

struct XtwCriticalSection {
  CRITICAL_SECTION cs;
  XtwCriticalSection(const XtwCriticalSection&) = delete;
  XtwCriticalSection& operator=(const XtwCriticalSection&) = delete;
  ~XtwCriticalSection() { DeleteCriticalSection(&cs); }
  XtwCriticalSection(): cs() { InitializeCriticalSection(&cs); }
};

struct XtwWaitableTimer {
  HANDLE timer;
  XtwWaitableTimer(const XtwWaitableTimer&) = delete;
  XtwWaitableTimer& operator=(const XtwWaitableTimer&) = delete;
  ~XtwWaitableTimer() { XT_ASSERT(CloseHandle(timer)); }
  XtwWaitableTimer() { XT_ASSERT((timer = CreateWaitableTimer(nullptr, FALSE, nullptr)) != nullptr) ; }
};

struct XtwEvent {
  HANDLE event;
  XtwEvent(const XtwEvent&) = delete;
  XtwEvent& operator=(const XtwEvent&) = delete;
  ~XtwEvent() { XT_ASSERT(CloseHandle(event)); }
  XtwEvent(): event() { XT_ASSERT((event = ::CreateEvent(nullptr, FALSE, FALSE, nullptr)) != nullptr); }
};

struct XtwWin32BlockingStream: public XtBlockingStream {
  XtwCriticalSection lock;
  const XtwEvent respondEvent;
  const XtwEvent controlEvent;
  XtBlockingStreamState state;
  XT_IMPLEMENT_CALLBACK_OVER_BLOCKING_STREAM();

  XtwWin32BlockingStream(bool secondary);
  ~XtwWin32BlockingStream();
  bool VerifyStreamCallback(HRESULT hr, const char* file, int line, const char* func, const char* expr);
};

#endif // _WIN32
#endif // _XT_WIN32_HPP