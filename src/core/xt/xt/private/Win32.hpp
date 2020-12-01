#ifndef XT_PRIVATE_WIN32_HPP
#define XT_PRIVATE_WIN32_HPP
#ifdef _WIN32
#define NOMINMAX 1
#include <xt/private/Shared.hpp>
#include <Windows.h>

struct XtPropVariant
{
  PROPVARIANT pv;
  ~XtPropVariant() { PropVariantClear(&pv); }
  XtPropVariant(): pv() { PropVariantInit(&pv); }
  XtPropVariant(XtPropVariant const&) = delete;
  XtPropVariant& operator=(XtPropVariant const&) = delete;
};

struct XtCriticalSection
{
  CRITICAL_SECTION cs;
  ~XtCriticalSection() { DeleteCriticalSection(&cs); }
  XtCriticalSection(): cs() { InitializeCriticalSection(&cs); }
  XtCriticalSection(XtCriticalSection const&) = delete;
  XtCriticalSection& operator=(XtCriticalSection const&) = delete;
};

struct XtEvent
{
  HANDLE event;
  XtEvent(XtEvent const&) = delete;
  XtEvent& operator=(XtEvent const&) = delete;
  ~XtEvent() { XT_ASSERT(CloseHandle(event)); }
  XtEvent(): event() { XT_ASSERT((event = CreateEvent(nullptr, FALSE, FALSE, nullptr)) != nullptr); }
};

struct XtWaitableTimer
{
  HANDLE timer;
  XtWaitableTimer(XtWaitableTimer const&) = delete;
  XtWaitableTimer& operator=(XtWaitableTimer const&) = delete;
  ~XtWaitableTimer() { XT_ASSERT(CloseHandle(timer)); }
  XtWaitableTimer() { XT_ASSERT((timer = CreateWaitableTimer(nullptr, FALSE, nullptr)) != nullptr) ; }
};

#endif // _WIN32
#endif // XT_PRIVATE_WIN32_HPP