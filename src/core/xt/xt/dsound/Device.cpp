#if XT_ENABLE_DSOUND
#include <xt/dsound/Shared.hpp>
#include <xt/dsound/Private.hpp>
#include <xt/private/Win32.hpp>

XtFault 
DSoundDevice::ShowControlPanel()
{ return DS_OK; }
XtFault
DSoundDevice::GetMix(XtBool* valid, XtMix* mix) const
{ return DS_OK; }

XtFault
DSoundDevice::GetChannelCount(XtBool output, int32_t* count) const
{
  if(output && _output == nullptr) return S_OK;
  *count = sizeof(XtiWfxChannelNames) / sizeof(char const*);
  return S_OK;
}

XtFault
DSoundDevice::SupportsAccess(XtBool interleaved, XtBool* supports) const
{ *supports = interleaved; return S_OK; }

XtFault
DSoundDevice::SupportsFormat(XtFormat const* format, XtBool* supports) const
{
  if(format->mix.rate < XtiDsMinSampleRate) return DS_OK;
  if(format->mix.rate > XtiDsMaxSampleRate) return DS_OK;
  if(_input.p == nullptr && format->channels.inputs > 0) return S_OK;
  if(_output.p == nullptr && format->channels.outputs > 0) return S_OK;
  return S_OK;
}

XtFault
DSoundDevice::GetBufferSize(XtFormat const* format, XtBufferSize* size) const
{
  size->min = XtiDsMinBufferMs;
  size->max = XtiDsMaxBufferMs;
  size->current = XtiDsDefaultBufferMs;
  return S_OK;
}

XtFault
DSoundDevice::GetChannelName(XtBool output, int32_t index, char* buffer, int32_t* size) const
{
  XtiCopyString(XtiWfxChannelNames[index], buffer, size);
  return S_OK;
}

#endif // XT_ENABLE_DSOUND