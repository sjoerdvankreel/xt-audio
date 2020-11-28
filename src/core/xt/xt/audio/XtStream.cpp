#include <xt/audio/XtStream.h>
#include <xt/Private.hpp>

XtFormat const* XT_CALL 
XtStreamGetFormat(XtStream const* s) 
{
  XT_ASSERT(s != nullptr);
  return &s->_params.format;
}

void XT_CALL 
XtStreamDestroy(XtStream* s) 
{ 
  XT_ASSERT(XtiCalledOnMainThread());
  delete s;
}

XtError XT_CALL 
XtStreamStop(XtStream* s) 
{
  XT_ASSERT(s != nullptr);
  XT_ASSERT(XtiCalledOnMainThread());
  return XtiCreateError(s->GetSystem(), s->Stop());
}

XtError XT_CALL 
XtStreamStart(XtStream* s) 
{
  XT_ASSERT(s != nullptr);
  XT_ASSERT(XtiCalledOnMainThread());
  return XtiCreateError(s->GetSystem(), s->Start());
}

XtError XT_CALL 
XtStreamGetFrames(XtStream const* s, int32_t* frames) 
{
  XT_ASSERT(s != nullptr);
  XT_ASSERT(frames != nullptr);
  *frames = 0;
  return XtiCreateError(s->GetSystem(), s->GetFrames(frames));
}

XtError XT_CALL 
XtStreamGetLatency(XtStream const* s, XtLatency* latency)
{
  XT_ASSERT(s != nullptr);
  XT_ASSERT(latency != nullptr);
  memset(latency, 0, sizeof(XtLatency));
  return XtiCreateError(s->GetSystem(), s->GetLatency(latency));
}