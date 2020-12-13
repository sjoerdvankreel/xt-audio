#ifndef XT_DSOUND_PRIVATE_HPP
#define XT_DSOUND_PRIVATE_HPP
#if XT_ENABLE_DSOUND

#include <xt/api/public/Enums.h>
#include <xt/private/Shared.hpp>
#include <xt/private/Win32.hpp>
#include <string>

inline double const
XtiDsMinBufferMs = 100.0;
inline double const
XtiDsMaxBufferMs = 5000.0;
inline double const
XtiDsDefaultBufferMs = 500.0;

inline double const
XtiDsMinSampleRate = 8000.0;
inline double const
XtiDsMaxSampleRate = 192000.0;

char const* 
XtiGetDSoundFaultText(XtFault fault);
XtCause 
XtiGetDSoundFaultCause(XtFault fault);

struct XtDsDeviceInfo
{
  GUID id;
  std::string name;
  bool output;
};

struct XtDsWaitableTimer
{
  HANDLE timer;
  XtDsWaitableTimer(XtDsWaitableTimer const&) = delete;
  XtDsWaitableTimer& operator=(XtDsWaitableTimer const&) = delete;
  ~XtDsWaitableTimer() { XT_ASSERT(CloseHandle(timer)); }
  XtDsWaitableTimer() { XT_ASSERT((timer = CreateWaitableTimer(nullptr, FALSE, nullptr)) != nullptr) ; }
};

#endif // XT_ENABLE_DSOUND
#endif // XT_DSOUND_PRIVATE_HPP