#ifndef XT_PRIVATE_HPP
#define XT_PRIVATE_HPP

#include "XtAudio.h"
#include <string>
#include <vector>
#include <memory>
#include <cstring>
#include <cstdarg>

#ifdef _WIN32
#define XT_SEPARATOR '\\'
#else
#define XT_SEPARATOR '/'
#endif // _WIN32

// ---- internal ----

#define XT_VERIFY_STREAM_CALLBACK(expr) \
  VerifyStreamCallback((expr), __FILE__, __LINE__, __func__, #expr)

#define XT_WAIT_TIMEOUT_MS 10000
#define XT_FAIL(m) XtiFail(__FILE__, __LINE__, __func__, m)
#define XT_ASSERT(c) ((c) || (XT_FAIL("Assertion failed: " #c), 0))
#define XT_TRACE(fmt, ...) XtiTrace(__FILE__, __LINE__, __func__, fmt, __VA_ARGS__)

static_assert(sizeof(XtCause) == 4);
static_assert(sizeof(XtSetup) == 4);
static_assert(sizeof(XtSystem) == 4);
static_assert(sizeof(XtSample) == 4);
static_assert(sizeof(XtCapabilities) == 4);

// ---- forward ----

#define XT_IMPLEMENT_CALLBACK_OVER_BLOCKING_STREAM() \
  XtFault Stop() override;                           \
  XtFault Start() override;                          \
  void RequestStop() override;

#define XT_IMPLEMENT_CALLBACK_STREAM(system)             \
  XtFault Stop() override;                               \
  XtFault Start() override;                              \
  XtFault GetFrames(int32_t* frames) const override;     \
  XtFault GetLatency(XtLatency* latency) const override; \
  XtSystem GetSystem() const override { return XtSystem ## system; }

#define XT_IMPLEMENT_BLOCKING_STREAM(system)             \
  void StopStream() override;                            \
  void StartStream() override;                           \
  void ProcessBuffer(bool prefill) override;             \
  XtFault GetFrames(int32_t* frames) const override;     \
  XtFault GetLatency(XtLatency* latency) const override; \
  XtSystem GetSystem() const override { return XtSystem ## system; }

#define XT_DECLARE_SERVICE(system, name)                                      \
struct name ## Service: public XtService {                                    \
  XtFault GetFormatFault() const override;                                    \
  XtCapabilities GetCapabilities() const override;                            \
  XtCause GetFaultCause(XtFault fault) const override;                        \
  XtFault GetDeviceCount(int32_t* count) const override;                      \
  const char* GetFaultText(XtFault fault) const override;                     \
  XtSystem GetSystem() const override { return XtSystem ## system; }          \
  XtFault OpenDevice(int32_t index, XtDevice** device) const override;        \
  XtFault OpenDefaultDevice(XtBool output, XtDevice** device) const override; \
};                                                                            \
static const name ## Service Service ## name;                                 \
const XtService* XtiService ## name = &Service ## name

#define XT_IMPLEMENT_DEVICE(system)                                                                 \
  XtFault ShowControlPanel() override;                                                              \
  XtFault GetMix(XtBool* valid, XtMix* mix) const override;                                         \
  XtFault GetName(char* buffer, int32_t* size) const override;                                      \
  XtSystem GetSystem() const override { return XtSystem ## system; }                                \
  XtFault GetChannelCount(XtBool output, int32_t* count) const override;                            \
  XtFault SupportsAccess(XtBool interleaved, XtBool* supports) const override;                      \
  XtFault GetBufferSize(const XtFormat* format, XtBufferSize* size) const override;                 \
  XtFault SupportsFormat(const XtFormat* format, XtBool* supports) const override;                  \
  XtFault GetChannelName(XtBool output, int32_t index, char* buffer, int32_t* size) const override; \
  XtFault OpenStream(const XtFormat* format, XtBool interleaved, double bufferSize,                 \
                     bool secondary, XtStreamCallback callback, void* user, XtStream** stream) override

// ---- internal ----

extern char* XtiId;
struct XtAggregate;
typedef uint32_t XtFault;
extern XtErrorCallback XtiErrorCallback;

enum class XtBlockingStreamState {
  Stopped,
  Starting,
  Started,
  Stopping,
  Closing,
  Closed
};

struct XtAggregateContext {
  int32_t index;
  XtAggregate* stream;
};

struct XtRingBuffer {
  int32_t end;
  int32_t full;
  int32_t begin;
  int32_t frames;
  int32_t channels;
  bool interleaved;
  int32_t sampleSize;
  mutable int32_t locked;
  std::vector<std::vector<char>> blocks;

  XtRingBuffer();
  XtRingBuffer(bool interleaved, int32_t frames, 
    int32_t channels, int32_t sampleSize);

  void Clear();
  void Lock() const;
  void Unlock() const;
  int32_t Full() const;
  int32_t Read(void* target, int32_t frames);
  int32_t Write(const void* source, int32_t frames);
};

struct XtIntermediateBuffers {
  std::vector<char> inputInterleaved;
  std::vector<char> outputInterleaved;
  std::vector<void*> inputNonInterleaved;
  std::vector<void*> outputNonInterleaved;
  std::vector<std::vector<char>> inputChannelsNonInterleaved;
  std::vector<std::vector<char>> outputChannelsNonInterleaved;
};

// ---- forward ----

struct XtService {
  virtual ~XtService() {};
  virtual XtSystem GetSystem() const = 0;
  virtual XtFault GetFormatFault() const = 0;
  virtual XtCapabilities GetCapabilities() const = 0;
  virtual XtCause GetFaultCause(XtFault fault) const = 0;
  virtual XtFault GetDeviceCount(int32_t* count) const = 0;
  virtual const char* GetFaultText(XtFault fault) const = 0;
  virtual XtFault OpenDevice(int32_t index, XtDevice** device) const = 0;
  virtual XtFault OpenDefaultDevice(XtBool output, XtDevice** device) const = 0;
};

struct XtStream {
  void* user;
  bool aggregated;
  XtFormat format;
  int32_t sampleSize;
  XtBool interleaved;
  XtBool canInterleaved;
  XtBool canNonInterleaved;
  int32_t aggregationIndex;
  XtXRunCallback xRunCallback;
  XtStreamCallback streamCallback;
  XtIntermediateBuffers intermediate;

  virtual ~XtStream() {};
  virtual void RequestStop();
  virtual XtFault Stop() = 0;
  virtual XtFault Start() = 0;
  virtual XtSystem GetSystem() const = 0;
  virtual bool IsBlocking() const { return false; }
  virtual XtFault GetFrames(int32_t* frames) const = 0;
  virtual XtFault GetLatency(XtLatency* latency) const = 0;
  void ProcessXRun();
  void ProcessCallback(const XtBuffer* buffer);
};

struct XtBlockingStream: public XtStream {
  const bool secondary;
  XtBlockingStream(bool secondary);
  virtual ~XtBlockingStream() {};
  virtual void StopStream() = 0;
  virtual void StartStream() = 0;
  virtual void ProcessBuffer(bool prefill) = 0;
  bool IsBlocking() const override { return true; }
};

struct XtAggregate: public XtStream {
  int32_t frames;
  XtSystem system;
  int32_t masterIndex;
  volatile int32_t running;
  XtIntermediateBuffers weave;
  std::vector<XtChannels> channels;
  volatile int32_t insideCallbackCount;
  std::vector<XtRingBuffer> inputRings; 
  std::vector<XtRingBuffer> outputRings;
  std::vector<XtAggregateContext> contexts;
  std::vector<std::unique_ptr<XtStream>> streams;

  virtual ~XtAggregate();
  virtual XtFault Stop();
  virtual XtFault Start();
  virtual XtSystem GetSystem() const;
  virtual XtFault GetFrames(int32_t* frames) const;
  virtual XtFault GetLatency(XtLatency* latency) const;
};

struct XtDevice {
  virtual ~XtDevice() {};
  virtual XtFault ShowControlPanel() = 0;
  virtual XtSystem GetSystem() const = 0;
  virtual XtFault GetMix(XtBool* valid, XtMix* mix) const = 0;
  virtual XtFault GetName(char* buffer, int32_t* size) const = 0;
  virtual XtFault GetChannelCount(XtBool output, int32_t* count) const = 0;
  virtual XtFault SupportsAccess(XtBool interleaved, XtBool* supports) const = 0;
  virtual XtFault SupportsFormat(const XtFormat* format, XtBool* supports) const = 0;
  virtual XtFault GetBufferSize(const XtFormat* format, XtBufferSize* size) const = 0;
  virtual XtFault GetChannelName(XtBool output, int32_t index, char* buffer, int32_t* size) const = 0;
  virtual XtFault OpenStream(const XtFormat* format, XtBool interleaved, double bufferSize, 
                             bool secondary, XtStreamCallback callback, void* user, XtStream** stream) = 0;
};

// ---- internal ----

void XtiTerminatePlatform();
bool XtiCalledOnMainThread();
void XtiInitPlatform(void* window);
int32_t XtiGetPopCount64(uint64_t x);
uint32_t XtiGetErrorFault(XtError error);
int32_t XtiGetSampleSize(XtSample sample);
int32_t XtiLockIncr(volatile int32_t* dest);
int32_t XtiLockDecr(volatile int32_t* dest);
XtError XtiCreateError(XtSystem system, XtFault fault);
bool XtiValidateFormat(XtSystem system, const XtFormat& format);
int32_t XtiCas(volatile int32_t* dest, int32_t exch, int32_t comp);
void XtiOutputString(const char* source, char* buffer, int32_t* size);
void XtiFail(const char* file, int line, const char* func, const char* message);
void XtiTrace(const char* file, int32_t line, const char* func, const char* format, ...);
void XtiVTrace(const char* file, int32_t line, const char* func, const char* format, va_list arg);

void XT_CALLBACK XtiSlaveCallback(const XtStream* stream, const XtBuffer* buffer, void* user);
void XT_CALLBACK XtiMasterCallback(const XtStream* stream, const XtBuffer* buffer, void* user);

#endif // XT_PRIVATE_HPP