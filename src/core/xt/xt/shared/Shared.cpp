#include <xt/api/XtAudio.h>
#include <xt/api/XtPrint.h>
#include <xt/shared/Shared.hpp>
#include <xt/private/Device.hpp>
#include <xt/shared/Services.hpp>
#include <xt/private/Platform.hpp>

#include <thread>
#include <cassert>
#include <cstring>

int32_t
XtiGetPopCount64(uint64_t x) 
{
  uint64_t const m1 = 0x5555555555555555;
  uint64_t const m2 = 0x3333333333333333;
  uint64_t const m4 = 0x0f0f0f0f0f0f0f0f;
  uint64_t const h01 = 0x0101010101010101;
  x -= (x >> 1) & m1;
  x = (x & m2) + ((x >> 2) & m2);
  x = (x + (x >> 4)) & m4;
  return (x * h01) >> 56;
}

uint32_t
XtiGetErrorFault(XtError error) 
{ 
  uint64_t result = error & 0x00000000FFFFFFFF;
  return static_cast<XtFault>(result); 
}

int32_t
XtiGetSampleSize(XtSample sample) 
{
  auto attrs = XtAudioGetSampleAttributes(sample);
  return attrs.size;
}

void
XtiAssert(XtLocation const& location, char const* msg)
{
  XtiTrace(location, msg);
  std::terminate();
}

XtServiceError
XtiGetServiceError(XtSystem system, XtFault fault)
{
  switch(system)
  {
  case XtSystemALSA: return XtiGetAlsaError(fault);
  case XtSystemJACK: return XtiGetJackError(fault);
  case XtSystemASIO: return XtiGetAsioError(fault);
  case XtSystemPulse: return XtiGetPulseError(fault);
  case XtSystemWASAPI: return XtiGetWasapiError(fault);
  case XtSystemDSound: return XtiGetDSoundError(fault);
  default: XT_ASSERT(false); return XtServiceError();
  }
}

bool
XtiCalledOnMainThread()
{ 
  auto platform = XtPlatform::instance;
  auto id = std::this_thread::get_id();
  return platform != nullptr && id == platform->_threadId;
}

XtError
XtiCreateError(XtSystem system, XtFault fault) 
{
  if(fault == 0) return 0;
  auto result = static_cast<XtError>(system) << 32ULL | fault;
  auto info = XtAudioGetErrorInfo(result);
  XT_TRACE(XtPrintErrorInfoToString(&info));
  return result;
}

void
XtiTrace(XtLocation const& location, char const* msg)
{
  auto platform = XtPlatform::instance;
  if(platform == nullptr || platform->_onError == nullptr) return;
  platform->_onError(&location, msg);
}

void
XtiInitBuffers(XtBuffers& buffers, XtSample sample, size_t channels, size_t frames)
{
  int32_t size = XtiGetSampleSize(sample);
  std::vector<uint8_t> channel(frames * size, 0);
  buffers.nonInterleaved = std::vector<void*>(channels, nullptr);
  buffers.interleaved = std::vector<uint8_t>(frames * channels * size, 0);
  buffers.channels = std::vector<std::vector<uint8_t>>(channels, channel);
  for(size_t i = 0; i < channels; i++)
    buffers.nonInterleaved[i] = &(buffers.channels[i][0]);
}

void
XtiInitIOBuffers(XtIOBuffers& buffers, XtFormat const* format, size_t frames)
{
  XtiInitBuffers(buffers.input, format->mix.sample, format->channels.inputs, frames);
  XtiInitBuffers(buffers.output, format->mix.sample, format->channels.outputs, frames);
}

void
XtiCopyString(char const* source, char* buffer, int32_t* size) 
{
  if(buffer == nullptr) return (*size = static_cast<int32_t>(strlen(source)) + 1), void();
  memcpy(buffer, source, static_cast<size_t>(*size) - 1);
  buffer[*size - 1] = '\0';
}

void
XtiDeinterleave(void** dst, void const* src, int32_t frames, int32_t channels, int32_t size)
{
  uint8_t** d = reinterpret_cast<uint8_t**>(dst);
  uint8_t const* s = static_cast<uint8_t const*>(src);
  for(int32_t f = 0; f < frames; f++)
    for(int32_t c = 0; c < channels; c++)
      memcpy(&d[c][f * size], &s[(f * channels + c) * size], size);
}

void
XtiInterleave(void* dst, void const* const* src, int32_t frames, int32_t channels, int32_t size)
{
  uint8_t* d = static_cast<uint8_t*>(dst);
  uint8_t const* const* s = reinterpret_cast<uint8_t const* const*>(src);
  for(int32_t f = 0; f < frames; f++)
    for(int32_t c = 0; c < channels; c++)
      memcpy(&d[(f * channels + c) * size], &s[c][f * size], size);
}

XtFault
XtiSupportsFormat(XtDevice const* device, XtFormat const* format)
{
  XtError error;
  XtBool supports;
  if((error = XtDeviceSupportsFormat(device, format, &supports)) != 0) return XtiGetErrorFault(error);
  if(!supports) return XtPlatform::instance->GetService(device->GetSystem())->GetFormatFault();
  return 0;
}

void
XtiZeroBuffer(void* buffer, XtBool interleaved, int32_t posFrames, int32_t channels, int32_t frames, int32_t sampleSize)
{
  size_t ss = sampleSize;
  size_t fs = channels * ss;
  if(frames > 0)
    if(interleaved)
      memset(static_cast<uint8_t*>(buffer) + posFrames * fs, 0, frames * fs);
    else for(int32_t i = 0; i < channels; i++)
      memset(static_cast<uint8_t**>(buffer)[i] + posFrames * ss, 0, frames * ss);
}

void 
XtiWeave(void* dst, void const* src, XtBool interleaved, int32_t dstChans, int32_t srcChans, int32_t dstChan, int32_t srcChan, int32_t frames, int32_t sampleSize)
{
  assert(dstChans > 0);
  assert(srcChans > 0);
  assert(0 <= dstChan && dstChan < dstChans);
  assert(0 <= srcChan && srcChan < srcChans);
  char** dn = static_cast<char**>(dst);
  uint8_t* di = static_cast<uint8_t*>(dst);
  auto si = static_cast<uint8_t const*>(src);
  auto sn = static_cast<uint8_t const* const*>(src);

  if(!interleaved) for(int32_t f = 0; f < frames; f++)
  {
      auto to = &dn[dstChan][f * sampleSize];
      auto from = &sn[srcChan][f * sampleSize];
      memcpy(to, from, sampleSize);
  }
  else for(int32_t f = 0; f < frames; f++)
  {
      auto to = &di[(f * dstChans + dstChan) * sampleSize];
      auto from = &si[(f * srcChans + srcChan) * sampleSize];
      memcpy(to, from, sampleSize);
  }
}