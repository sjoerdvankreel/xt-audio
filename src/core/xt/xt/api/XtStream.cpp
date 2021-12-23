#include <xt/api/XtStream.h>
#include <xt/shared/Shared.hpp>
#include <xt/private/Stream.hpp>

#include <cstring>

XtBool XT_CALL
XtStreamIsRunning(XtStream const* s)
{
  XT_ASSERT_API(s != nullptr);
  return s->IsRunning();
}

void* XT_CALL
XtStreamGetHandle(XtStream const* s)
{
  XT_ASSERT_API(s != nullptr);
  return s->GetHandle();
}

XtFormat const* XT_CALL
XtStreamGetFormat(XtStream const* s)
{
  XT_ASSERT_API(s != nullptr);
  return &s->_params.format;
}

XT_API XtBool XT_CALL
XtStreamIsInterleaved(XtStream const* s)
{
  XT_ASSERT_API(s != nullptr);
  return s->_params.stream.interleaved;
}

void XT_CALL
XtStreamDestroy(XtStream* s) 
{ 
  XT_ASSERT_VOID_API(XtiCalledOnMainThread());
  delete s;
}

void XT_CALL 
XtStreamStop(XtStream* s) 
{
  XT_ASSERT_VOID_API(s != nullptr);
  XT_ASSERT_VOID_API(XtiCalledOnMainThread());
  s->Stop();
}

XtError XT_CALL 
XtStreamStart(XtStream* s) 
{
  XT_ASSERT_API(s != nullptr);
  XT_ASSERT_API(XtiCalledOnMainThread());
  return XtiCreateError(s->GetSystem(), s->Start());
}

XtError XT_CALL 
XtStreamGetFrames(XtStream const* s, int32_t* frames) 
{
  XT_ASSERT_API(s != nullptr);
  XT_ASSERT_API(frames != nullptr);
  *frames = 0;
  return XtiCreateError(s->GetSystem(), s->GetFrames(frames));
}

XtError XT_CALL 
XtStreamGetLatency(XtStream const* s, XtLatency* latency)
{
  XT_ASSERT_API(s != nullptr);
  XT_ASSERT_API(latency != nullptr);
  memset(latency, 0, sizeof(XtLatency));
  return XtiCreateError(s->GetSystem(), s->GetLatency(latency));
}