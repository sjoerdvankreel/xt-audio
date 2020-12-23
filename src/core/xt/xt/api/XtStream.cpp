#include <xt/api/XtStream.h>
#include <xt/shared/Shared.hpp>
#include <xt/private/Stream.hpp>

#include <cstring>

void* XT_CALL
XtStreamGetHandle(XtStream* s)
{
  XT_ASSERT(s != nullptr);
  return s->GetHandle();
}

void XT_CALL 
XtStreamStop(XtStream* s) 
{
  XT_ASSERT(s != nullptr);
  XT_ASSERT(XtiCalledOnMainThread());
  s->Stop();
}

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

XtBool XT_CALL
XtStreamIsRunning(XtStream const* s)
{
  XT_ASSERT(s != nullptr);
  return s->IsRunning();
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