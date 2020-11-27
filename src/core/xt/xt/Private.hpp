#ifndef XT_PRIVATE_HPP
#define XT_PRIVATE_HPP

#include <xt/XtAudio.h>
#include <xt/private/Shared.hpp>
#include <xt/private/Device.hpp>
#include <xt/private/Service.hpp>
#include <string>
#include <vector>
#include <memory>
#include <cstring>
#include <cstdarg>

// ---- internal ----

#define XT_VERIFY_ON_BUFFER(expr) \
  VerifyOnBuffer((expr), __FILE__, __LINE__, __func__, #expr)

#define XT_WAIT_TIMEOUT_MS 10000
#define XT_FAIL(m) XtiFail(__FILE__, __LINE__, __func__, m)
#define XT_ASSERT(c) ((c) || (XT_FAIL("Assertion failed: " #c), 0))
#define XT_TRACE(msg) XtiTrace(__FILE__, __LINE__, __func__, msg)

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

// ---- internal ----

struct XtAggregate;

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

struct XtStream {
  void* user;
  bool aggregated;
  XtFormat format;
  int32_t sampleSize;
  XtBool interleaved;
  XtBool canInterleaved;
  XtBool canNonInterleaved;
  int32_t aggregationIndex;
  XtOnXRun onXRun;
  XtOnBuffer onBuffer;
  XtIntermediateBuffers intermediate;

  virtual ~XtStream() {};
  virtual void RequestStop();
  virtual XtFault Stop() = 0;
  virtual XtFault Start() = 0;
  virtual XtSystem GetSystem() const = 0;
  virtual bool IsBlocking() const { return false; }
  virtual XtFault GetFrames(int32_t* frames) const = 0;
  virtual XtFault GetLatency(XtLatency* latency) const = 0;
  void OnXRun();
  void OnBuffer(const XtBuffer* buffer);
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

// ---- internal ----

void XtiFail(const char* file, int line, const char* func, const char* message);
void XtiTrace(const char* file, int32_t line, const char* func, const char* message);

void XT_CALLBACK XtiOnSlaveBuffer(const XtStream* stream, const XtBuffer* buffer, void* user);
void XT_CALLBACK XtiOnMasterBuffer(const XtStream* stream, const XtBuffer* buffer, void* user);

#endif // XT_PRIVATE_HPP