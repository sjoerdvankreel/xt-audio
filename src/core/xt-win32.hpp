#ifndef _XT_WIN32_HPP
#define _XT_WIN32_HPP
#ifdef _WIN32

#include "xt-private.hpp"
#include <windows.h>
#include <atlbase.h>
#include <mmsystem.h>
#include <mmreg.h>
#include <ks.h>
#include <ksmedia.h>
#include <string>

/* Copyright (C) 2015-2020 Sjoerd van Kreel.
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

// ---- win32 ----

#define XT_VERIFY_COM(e)                   \
do {                                       \
  if(FAILED(hr = (e)))                     \
    return XT_TRACE(XtLevelError, #e), hr; \
} while(0)

extern const XtService* XtiServiceAsio;
extern const XtService* XtiServiceDSound;
extern const XtService* XtiServiceWasapi;
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

struct XtwWin32Stream: public XtManagedStream {
  XtStreamState state;
  XtwCriticalSection lock;
  const XtwEvent respondEvent;
  const XtwEvent controlEvent;
  XT_IMPLEMENT_STREAM_CONTROL();

  XtwWin32Stream(bool secondary);
  ~XtwWin32Stream();
  virtual void RequestStop();
  bool VerifyStreamCallback(HRESULT hr, const char* file, int line, const char* func, const char* expr);
};

#endif // _WIN32
#endif // _XT_WIN32_HPP
