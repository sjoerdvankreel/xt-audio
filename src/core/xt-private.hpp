#ifndef _XT_PRIVATE_HPP
#define _XT_PRIVATE_HPP

#include "xt-audio.h"
#include <string>
#include <cstring>
#include <cstdarg>

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
#ifdef _WIN32
#define XT_SEPARATOR '\\'
#else
#define XT_SEPARATOR '/'
#endif // _WIN32

// ---- internal ----

#define XT_VERIFY_STREAM_CALLBACK(expr) \
  VerifyStreamCallback((expr), XT_FILE, __LINE__, __func__, #expr)

#define XT_WAIT_TIMEOUT_MS 10000
#define XT_FAIL(m) XtiFail(XT_FILE, __LINE__, __func__, m)
#define XT_ASSERT(c) ((c) || (XT_FAIL("Assertion failed: " #c), 0))
#define XT_FILE (strrchr(__FILE__, XT_SEPARATOR) ? strrchr(__FILE__, XT_SEPARATOR) + 1 : __FILE__)
#define XT_TRACE(l, m, ...) XtiTrace(l, XT_FILE, __LINE__, __func__, m, __VA_ARGS__)

static_assert(sizeof(XtLevel) == 4, "sizeof(XtLevel) == 4");
static_assert(sizeof(XtCause) == 4, "sizeof(XtCause) == 4");
static_assert(sizeof(XtSetup) == 4, "sizeof(XtSetup) == 4");
static_assert(sizeof(XtSystem) == 4, "sizeof(XtSystem) == 4");
static_assert(sizeof(XtSample) == 4, "sizeof(XtSample) == 4");
static_assert(sizeof(XtCapabilities) == 4, "sizeof(XtCapabilities) == 4");

// ---- forward ----

#define XT_IMPLEMENT_STREAM_CONTROL() \
  XtFault Stop();                     \
  XtFault Start();

#define XT_IMPLEMENT_STREAM(name)               \
  XtFault GetFrames(int32_t* frames) const;     \
  XtFault GetLatency(XtLatency* latency) const; \
  XtSystem GetSystem() const { return XtSystem ## name; }

#define XT_DECLARE_SERVICE(name)                                     \
struct name ## Service: public XtService {                           \
  const char* GetName() const;                                       \
  XtFault GetFormatFault() const;                                    \
  XtCapabilities GetCapabilities() const;                            \
  XtCause GetFaultCause(XtFault fault) const;                        \
  XtFault GetDeviceCount(int32_t* count) const;                      \
  const char* GetFaultText(XtFault fault) const;                     \
  XtSystem GetSystem() const { return XtSystem ## name; }            \
  XtFault OpenDevice(int32_t index, XtDevice** device) const;        \
  XtFault OpenDefaultDevice(XtBool output, XtDevice** device) const; \
};                                                                   \
static const name ## Service Service ## name;                        \
const XtService* XtiService ## name = &Service ## name

#define XT_IMPLEMENT_DEVICE(name)                                          \
  XtFault ShowControlPanel();                                              \
  XtFault GetMix(XtMix** mix) const;                                       \
  XtFault GetName(char** name) const;                                      \
  XtSystem GetSystem() const { return XtSystem ## name; }                  \
  XtFault GetChannelCount(XtBool output, int32_t* count) const;            \
  XtFault GetBuffer(const XtFormat* format, XtBuffer* buffer) const;       \
  XtFault SupportsAccess(XtBool interleaved, XtBool* supports) const;      \
  XtFault SupportsFormat(const XtFormat* format, XtBool* supports) const;  \
  XtFault GetChannelName(XtBool output, int32_t index, char** name) const; \
  XtFault OpenStream(const XtFormat* format, double bufferSize, XtStreamCallback callback, void* user, XtStream** stream)

// ---- internal ----

enum XtStreamState {
  XtStreamStateStopped,
  XtStreamStateStarting,
  XtStreamStateStarted,
  XtStreamStateStopping,
  XtStreamStateClosing,
  XtStreamStateClosed
};

typedef uint32_t XtFault;
extern char* XtiId;
extern XtTraceCallback XtiTraceCallback;
extern XtFatalCallback XtiFatalCallback;

// ---- forward ----

struct XtStream {
  void* user;
  XtFormat format;
  XtStreamCallback callback;
  virtual ~XtStream() {};
  virtual XtFault Stop() = 0;
  virtual XtFault Start() = 0;
  virtual XtSystem GetSystem() const = 0;
  virtual XtFault GetFrames(int32_t* frames) const = 0;
  virtual XtFault GetLatency(XtLatency* latency) const = 0;
};

struct XtService {
  virtual ~XtService() {};
  virtual XtSystem GetSystem() const = 0;
  virtual const char* GetName() const = 0;
  virtual XtFault GetFormatFault() const = 0;
  virtual XtCapabilities GetCapabilities() const = 0;
  virtual XtCause GetFaultCause(XtFault fault) const = 0;
  virtual XtFault GetDeviceCount(int32_t* count) const = 0;
  virtual const char* GetFaultText(XtFault fault) const = 0;
  virtual XtFault OpenDevice(int32_t index, XtDevice** device) const = 0;
  virtual XtFault OpenDefaultDevice(XtBool output, XtDevice** device) const = 0;
};

struct XtDevice {
  virtual ~XtDevice() {};
  virtual XtFault ShowControlPanel() = 0;
  virtual XtSystem GetSystem() const = 0;
  virtual XtFault GetMix(XtMix** mix) const = 0;
  virtual XtFault GetName(char** name) const = 0;
  virtual XtFault GetChannelCount(XtBool output, int32_t* count) const = 0;
  virtual XtFault GetBuffer(const XtFormat* format, XtBuffer* buffer) const = 0;
  virtual XtFault SupportsAccess(XtBool interleaved, XtBool* supports) const = 0;
  virtual XtFault SupportsFormat(const XtFormat* format, XtBool* supports) const = 0;
  virtual XtFault GetChannelName(XtBool output, int32_t index, char** name) const = 0;
  virtual XtFault OpenStream(const XtFormat* format, double bufferSize, XtStreamCallback callback, void* user, XtStream** stream) = 0;
};

// ---- internal ----

void XtiTerminatePlatform();
bool XtiCalledOnMainThread();
void XtiInitPlatform(void* window);
int32_t XtiGetPopCount64(uint64_t x);
XtSystem XtiSetupToSystem(XtSetup setup);
XtSystem XtiIndexToSystem(int32_t index);
int32_t XtiGetSampleSize(XtSample sample);
std::string XtiTryGetDeviceName(const XtDevice* d);
XtError XtiCreateError(XtSystem system, XtFault fault);
bool XtiValidateFormat(XtSystem system, const XtFormat& format);
void XtiFail(const char* file, int line, const char* func, const char* message);
void XtiTrace(XtLevel level, const char* file, int32_t line, const char* func, const char* format, ...);
void XtiVTrace(XtLevel level, const char* file, int32_t line, const char* func, const char* format, va_list arg);
void XtiInterleave(void* dest, void* source, int32_t frames, int32_t channels, int32_t sampleSize, int32_t channel);
void XtiDeinterleave(void* dest, void* source, int32_t frames, int32_t channels, int32_t sampleSize, int32_t channel);

#endif // _XT_PRIVATE_HPP
