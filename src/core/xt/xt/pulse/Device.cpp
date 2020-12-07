#if XT_ENABLE_PULSE
#include <xt/api/private/Platform.hpp>
#include <xt/pulse/Shared.hpp>
#include <pulse/pulseaudio.h>
#include <cmath>

PulseDevice::
PulseDevice(bool output): 
_output(output) { }

XtFault
PulseDevice::ShowControlPanel()
{ return 0; }
XtFault 
PulseDevice::SupportsAccess(XtBool interleaved, XtBool* supports) const 
{ *supports = interleaved; return PA_OK; }
XtFault 
PulseDevice::GetChannelCount(XtBool output, int32_t* count) const 
{ *count = _output != output? 0: PA_CHANNEL_POSITION_MAX; return PA_OK; }

XtFault 
PulseDevice::GetMix(XtBool* valid, XtMix* mix) const
{
  *valid = XtTrue;
  mix->rate = XtiPaDefaultRate;
  mix->sample = XtiPaDefaultSample;
  return PA_OK;
}

XtFault 
PulseDevice::GetBufferSize(XtFormat const* format, XtBufferSize* size) const
{
  size->min = XtiPaMinBufferSize;
  size->max = XtiPaMaxBufferSize;
  size->current = XtiPaDefaultBufferSize;
  return PA_OK;
}

XtFault 
PulseDevice::SupportsFormat(XtFormat const* format, XtBool* supports) const
{
  pa_sample_format pulse;
  if(format->mix.rate < XtiPaMinRate) return PA_OK;
  if(format->mix.rate > XtiPaMaxRate) return PA_OK;
  if(format->channels.inputs > 0 && _output) return PA_OK;
  if(format->channels.outputs > 0 && !_output) return PA_OK;
  if(format->channels.inputs >= PA_CHANNEL_POSITION_MAX) return PA_OK;
  if(format->channels.outputs >= PA_CHANNEL_POSITION_MAX) return PA_OK;
  for(int32_t i = PA_CHANNEL_POSITION_MAX; i < 64; i++)
    if(format->channels.inMask & (1ULL << i) || format->channels.outMask & (1ULL << i))
      return PA_OK;
  *supports = XtTrue;
  return PA_OK;
}

XtFault 
PulseDevice::GetChannelName(XtBool output, int32_t index, char* buffer, int32_t* size) const
{
  auto pos = static_cast<pa_channel_position_t>(index);
  auto text = pa_channel_position_to_pretty_string(pos);
  XtiCopyString(text, buffer, size);
  return PA_OK;
}

XtFault 
PulseDevice::OpenStreamCore(XtDeviceStreamParams const* params, bool secondary, void* user, XtStream** stream)
{
  pa_simple* pa;
  int fault = PA_OK;

  pa_sample_spec spec;
  spec.rate = params->format.mix.rate;
  auto const& channels = params->format.channels;
  spec.channels = channels.inputs + channels.outputs;
  spec.format = XtiSampleToPulse(params->format.mix.sample);
  
  pa_channel_map map;
  uint64_t mask = channels.inMask? channels.inMask: channels.outMask;
  if(mask != 0)
  {
    pa_channel_map_init(&map);
    map.channels = spec.channels;
    for(int32_t i = 0, j = 0; i < 64; i++)
      if(mask & (1ULL << i))
        map.map[j++] = static_cast<pa_channel_position_t>(i);
  } else
    pa_channel_map_init_extend(&map, spec.channels, PA_CHANNEL_MAP_DEFAULT);
  
  double df = params->bufferSize / 1000.0 * params->format.mix.rate;
  int32_t frames = static_cast<int32_t>(std::ceil(df));
  int32_t sampleSize = XtiGetSampleSize(params->format.mix.sample);
  int32_t frameSize = (channels.inputs + channels.outputs) * sampleSize;
  auto id = XtPlatform::instance->_id.c_str();
  auto dir = _output? PA_STREAM_PLAYBACK: PA_STREAM_RECORD;
  if((pa = pa_simple_new(nullptr, id, dir, nullptr, id, &spec, &map, nullptr, &fault)) == nullptr) return fault;
  *stream = new PulseStream(secondary, XtPaSimple(pa), _output, frames, frameSize);
  return PA_OK;
}

#endif // XT_ENABLE_PULSE