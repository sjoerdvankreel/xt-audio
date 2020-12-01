#ifndef XT_PRIVATE_HPP
#define XT_PRIVATE_HPP

#include <xt/XtAudio.h>
#include <xt/api/private/Device.hpp>
#include <xt/api/private/Stream.hpp>
#include <xt/api/private/Service.hpp>
#include <xt/private/BlockingStream.hpp>
#include <xt/private/Shared.hpp>
#include <string>
#include <vector>
#include <memory>
#include <atomic>
#include <cstring>
#include <cstdarg>

// ---- forward ----

// ---- internal ----

struct XtAggregate;

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
  mutable std::atomic<int32_t> locked;
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

  XtRingBuffer(XtRingBuffer const&);
  XtRingBuffer& operator=(XtRingBuffer const&);
};

struct XtAggregate: public XtStream {
  int32_t frames;
  XtSystem system;
  int32_t masterIndex;
  std::atomic<int32_t> running;
  XtIOBuffers _weave;
  std::vector<XtChannels> channels;
  std::atomic<int32_t> insideCallbackCount;
  std::vector<XtRingBuffer> inputRings; 
  std::vector<XtRingBuffer> outputRings;
  std::vector<XtAggregateContext> contexts;
  std::vector<std::unique_ptr<XtBlockingStream>> streams;

  virtual ~XtAggregate();
  XT_IMPLEMENT_STREAM();
};

void XT_CALLBACK XtiOnSlaveBuffer(const XtStream* stream, const XtBuffer* buffer, void* user);
void XT_CALLBACK XtiOnMasterBuffer(const XtStream* stream, const XtBuffer* buffer, void* user);

#endif // XT_PRIVATE_HPP