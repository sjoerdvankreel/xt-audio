#ifndef XT_DSOUND_PRIVATE_HPP
#define XT_DSOUND_PRIVATE_HPP
#if XT_ENABLE_DSOUND

#include <xt/api/public/Enums.h>
#include <xt/private/Shared.hpp>
#include <xt/private/Win32.hpp>
#include <string>
#include <cstring>

inline int const
XtiDsWakeUpsPerBuffer = 8;

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

char const* 
XtiGetDSoundFaultText(XtFault fault);
XtCause 
XtiGetDSoundFaultCause(XtFault fault);

inline DWORD 
XtiDsWrapAround(int32_t bytes, int32_t bufferSize);
inline UINT
XtiDsGetTimerPeriod(int32_t bufferFrames, int32_t rate);
inline bool
XtiDsInsideSafetyGap(DWORD read, DWORD write, DWORD lockPosition);
inline void
XtiDsSplitBufferParts(std::vector<uint8_t>& buffer, void* part1, DWORD size1, void* part2, DWORD size2);
inline void
XtiDsCombineBufferParts(std::vector<uint8_t>& buffer, void* part1, DWORD size1, void* part2, DWORD size2);

inline DWORD 
XtiDsWrapAround(int32_t bytes, int32_t bufferSize)
{ return bytes >= 0? bytes: bytes + bufferSize; }
inline UINT
XtiDsGetTimerPeriod(int32_t bufferFrames, int32_t rate)
{ return static_cast<UINT>(bufferFrames * 1000.0 / rate / XtiDsWakeUpsPerBuffer); }

inline bool
XtiDsInsideSafetyGap(DWORD read, DWORD write, DWORD lockPosition)
{
  if(read < write && read <= lockPosition && lockPosition < write) return true;
  if(read > write && (read <= lockPosition || lockPosition < write)) return true;
  return false;
}

inline void
XtiDsSplitBufferParts(std::vector<uint8_t>& buffer, void* part1, DWORD size1, void* part2, DWORD size2)
{
  memcpy(part1, &buffer[0], size1);
  if(size2 != 0) memcpy(part2, &buffer[size1], size2);
}

inline void
XtiDsCombineBufferParts(std::vector<uint8_t>& buffer, void* part1, DWORD size1, void* part2, DWORD size2)
{
  memcpy(&buffer[0], part1, size1);
  if(size2 != 0) memcpy(&buffer[size1], part2, size2);
}

#endif // XT_ENABLE_DSOUND
#endif // XT_DSOUND_PRIVATE_HPP