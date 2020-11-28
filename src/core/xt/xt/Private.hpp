#ifndef XT_PRIVATE_HPP
#define XT_PRIVATE_HPP

#include <xt/XtAudio.h>
#include <xt/private/Shared.hpp>
#include <xt/private/Device.hpp>
#include <xt/private/Stream.hpp>
#include <xt/private/Structs.hpp>
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

// ---- forward ----

#define XT_IMPLEMENT_CALLBACK_OVER_BLOCKING_STREAM() \
  XtFault Stop() override;                           \
  XtFault Start() override;                          \
  void RequestStop() override;

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
  std::vector<std::vector<uint8_t>> blocks;

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

struct XtBlockingStream: 
public XtStream 
{
  int32_t _index;
  bool _aggregated;
  bool const _secondary;

  virtual void OnXRun() const override;
  XtBlockingStream(bool secondary):
  _index(-1), _aggregated(false), _secondary(secondary) {}

  virtual void StopStream() = 0;
  virtual void StartStream() = 0;  
  virtual void RequestStop() = 0;
  virtual void ProcessBuffer(bool prefill) = 0;
};

struct XtAggregate: public XtStream {
  int32_t frames;
  XtSystem system;
  int32_t masterIndex;
  volatile int32_t running;
  XtIOBuffers _weave;
  std::vector<XtChannels> channels;
  volatile int32_t insideCallbackCount;
  std::vector<XtRingBuffer> inputRings; 
  std::vector<XtRingBuffer> outputRings;
  std::vector<XtAggregateContext> contexts;
  std::vector<std::unique_ptr<XtBlockingStream>> streams;

  virtual ~XtAggregate();
  virtual XtFault Stop();
  virtual XtFault Start();
  virtual XtSystem GetSystem() const;
  virtual XtFault GetFrames(int32_t* frames) const;
  virtual XtFault GetLatency(XtLatency* latency) const;
};

void XT_CALLBACK XtiOnSlaveBuffer(const XtStream* stream, const XtBuffer* buffer, void* user);
void XT_CALLBACK XtiOnMasterBuffer(const XtStream* stream, const XtBuffer* buffer, void* user);

#endif // XT_PRIVATE_HPP