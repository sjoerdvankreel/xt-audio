#if XT_ENABLE_DSOUND
#include <xt/dsound/Shared.hpp>
#include <xt/dsound/Private.hpp>

DSoundStream::
DSoundStream(bool secondary):
XtBlockingStream(secondary), _timer() { }

XtFault
DSoundStream::GetFrames(int32_t* frames) const 
{ *frames = _bufferFrames; return S_OK; }
XtFault
DSoundStream::GetLatency(XtLatency* latency) const
{ return S_OK; }

void
DSoundStream::StopStream()
{
  if(_inputBuffer) XT_ASSERT(SUCCEEDED(_inputBuffer->Stop()));
  else XT_ASSERT(SUCCEEDED(_outputBuffer->Stop()));
  if(!_secondary)
  {
    UINT period = XtiDsGetTimerPeriod(_bufferFrames, _params.format.mix.rate);
    XT_ASSERT(CancelWaitableTimer(_timer.timer));
    XT_ASSERT(SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_NORMAL));
    XT_ASSERT(timeEndPeriod(period / 2) == TIMERR_NOERROR);
  }
  _xtProcessed = 0;
  _dsProcessed = 0;
  _previousPosition = 0;
}

void
DSoundStream::StartStream()
{
  if(!_secondary)
  {
    LARGE_INTEGER due;
    due.QuadPart = -1;
    UINT period = XtiDsGetTimerPeriod(_bufferFrames, _params.format.mix.rate);
    XT_ASSERT(timeBeginPeriod(period / 2) == TIMERR_NOERROR);
    XT_ASSERT(SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL));
    XT_ASSERT(SetWaitableTimer(_timer.timer, &due, period, nullptr, nullptr, TRUE));
  }
  memset(_audio.data(), 0, _bufferFrames * _frameSize);
  if(_inputBuffer) XT_ASSERT(SUCCEEDED(_inputBuffer->Start(DSCBSTART_LOOPING)));
  else XT_ASSERT(SUCCEEDED(_outputBuffer->Play(0, 0, DSBPLAY_LOOPING)));
}

#endif // XT_ENABLE_DSOUND