#ifndef XT_SHARED_SHARED_HPP
#define XT_SHARED_SHARED_HPP

#include <xt/api/Enums.h>
#include <xt/api/Shared.h>
#include <xt/api/Structs.h>
#include <xt/shared/Structs.hpp>

#include <atomic>
#include <cstdint>

typedef uint32_t XtFault;

#define XT_STRINGIFY(s) #s
#define XT_LOCATION {__FILE__,  __func__, __LINE__}

#define XT_TRACE(m) XtiTrace(XT_LOCATION, m)
#define XT_ASSERT(c) ((c) || (XtiAssert(XT_LOCATION, #c), 0))
#define XT_TRACE_IF(c) (!(c) || (XtiTrace(XT_LOCATION, #c), 0))
#define XT_VERIFY(e, f) do { auto e_ = (e); if(!(e)) { XT_TRACE(#e); return f; } } while(0)

bool
XtiCalledOnMainThread();
int32_t
XtiGetPopCount64(uint64_t x);
uint32_t
XtiGetErrorFault(XtError error);
int32_t
XtiGetSampleSize(XtSample sample);
XtError
XtiCreateError(XtSystem system, XtFault fault);
XtServiceError
XtiGetServiceError(XtSystem system, XtFault fault);
void
XtiTrace(XtLocation const& location, char const* msg);
void
XtiAssert(XtLocation const& location, char const* msg);
void
XtiCopyString(char const* source, char* buffer, int32_t* size);
XtFault
XtiSupportsFormat(XtDevice const* device, XtFormat const* format);
inline bool
XtiCompareExchange(std::atomic_int& value, int32_t expected, int32_t desired);
void 
XtiInitIOBuffers(XtIOBuffers& buffers, XtFormat const* format, size_t frames);
void
XtiInitBuffers(XtBuffers& buffers, XtSample sample, size_t channels, size_t frames);
void
XtiDeinterleave(void** dst, void const* src, int32_t frames, int32_t channels, int32_t size);
void
XtiInterleave(void* dst, void const* const* src, int32_t frames, int32_t channels, int32_t size);
void
XtiZeroBuffer(void* buffer, XtBool interleaved, int32_t posFrames, int32_t channels, int32_t frames, int32_t sampleSize);
void 
XtiWeave(void* dst, void const* src, XtBool interleaved, int32_t dstChans, int32_t srcChans, int32_t dstChan, int32_t srcChan, int32_t frames, int32_t sampleSize);

template <class Rollback>
inline XtGuard<Rollback> XtiGuard(Rollback rollback)
{ return XtGuard<Rollback>(rollback); };

bool
XtiCompareExchange(std::atomic_int& value, int32_t expected, int32_t desired)
{ return value.compare_exchange_strong(expected, desired); }

template <class OnEmulatedBuffer>
XtFault
XtiOnBuffer(XtOnBufferParams* params, OnEmulatedBuffer onEmulated)
{
  XtBuffer converted = *params->buffer;
  XtIOBuffers* buffers = params->buffers;
  XtBuffer const* buffer = params->buffer;
  XtFault result = static_cast<XtFault>(-1);
  XtChannels const* channels = &params->format->channels;

  int32_t inputs = channels->inputs;
  int32_t outputs = channels->outputs;
  int32_t size = XtiGetSampleSize(params->format->mix.sample);
  auto interleavedIn = buffers->input.interleaved.data();
  auto interleavedOut = buffers->output.interleaved.data();
  auto nonInterleavedIn = buffers->input.nonInterleaved.data();
  auto nonInterleavedOut = buffers->output.nonInterleaved.data();
  bool haveInput = buffer->input != nullptr && buffer->frames > 0;
  bool haveOutput = buffer->output != nullptr && buffer->frames > 0;
  auto nonInterleavedBufferOut = static_cast<void**>(buffer->output);
  auto nonInterleavedBufferIn = static_cast<void const* const*>(buffer->input);

  if(!params->emulated) 
  {
    converted.input = haveInput? buffer->input: nullptr;
    converted.output = haveOutput? buffer->output: nullptr;
    result = onEmulated(&converted);
  } else if(!params->interleaved) 
  {
    converted.input = haveInput? nonInterleavedIn: nullptr;
    converted.output = haveOutput? nonInterleavedOut: nullptr;
    if(haveInput) XtiDeinterleave(nonInterleavedIn, buffer->input, buffer->frames, inputs, size);
    result = onEmulated(&converted);
    if(haveOutput) XtiInterleave(buffer->output, nonInterleavedOut, buffer->frames, outputs, size);
  } else
  {
    converted.input = haveInput? interleavedIn: nullptr;
    converted.output = haveOutput? interleavedOut: nullptr;
    if(haveInput) XtiInterleave(interleavedIn, nonInterleavedBufferIn, buffer->frames, inputs, size);
    result = onEmulated(&converted);
    if(haveOutput) XtiDeinterleave(nonInterleavedBufferOut, interleavedOut, buffer->frames, outputs, size);
  }
  return result;
}

#endif // XT_SHARED_SHARED_HPP