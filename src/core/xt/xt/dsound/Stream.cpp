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
  if(_inputBuffer) XT_ASSERT_COM(_inputBuffer->Stop());
  else XT_ASSERT_COM(_outputBuffer->Stop());
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
  memset(_audio.data(), 0, _audio.size());
  if(_inputBuffer) XT_ASSERT_COM(_inputBuffer->Start(DSCBSTART_LOOPING));
  else XT_ASSERT_COM(_outputBuffer->Play(0, 0, DSBPLAY_LOOPING));
}

void 
DSoundStream::ProcessBuffer(bool prefill)
{  
  void* audio1;
  void* audio2;
  DWORD size1, size2, read, write;
  DWORD bufferBytes = static_cast<DWORD>(_audio.size());

  if(!prefill && !_secondary)
  {
    DWORD bufferMillis = static_cast<DWORD>(_bufferFrames * 1000.0 / _params.format.mix.rate);
    XT_ASSERT(WaitForSingleObject(_timer.timer, bufferMillis) == WAIT_OBJECT_0);
  }
  
  XtBuffer buffer = { 0 };
  if(prefill && _outputBuffer)
  {
    if(!XT_VERIFY_ON_BUFFER(_outputBuffer->Lock(0, bufferBytes, &audio1, &size1, &audio2, &size2, 0))) return;
    if(size2 == 0)
    {
      buffer.input = nullptr;
      buffer.output = audio1;
      buffer.frames = _bufferFrames;
      OnBuffer(&buffer);
    } else
    {
      buffer.input = nullptr;
      buffer.output = _audio.data();
      buffer.frames = _bufferFrames;
      OnBuffer(&buffer);
      XtiDsSplitBufferParts(_audio, audio1, size1, audio2, size2);
    }
    if(!XT_VERIFY_ON_BUFFER(_outputBuffer->Unlock(audio1, size1, audio2, size2))) return;
    _xtProcessed += bufferBytes;
    return;
  }

  if(_inputBuffer && !prefill)
  {
    if(!XT_VERIFY_ON_BUFFER(_inputBuffer->GetCurrentPosition(&write, &read))) return;
    int32_t gap = XtiDsWrapAround(write - read, bufferBytes);
    _dsProcessed += XtiDsWrapAround(write - _previousPosition, bufferBytes);
    DWORD lockPosition = _xtProcessed % bufferBytes;
    int32_t available = static_cast<int32_t>(_dsProcessed - _xtProcessed - gap);
    _previousPosition = write;
    if(available <= 0) return;
    if(XtiDsInsideSafetyGap(read, write, lockPosition))
    {
      XT_VERIFY_ON_BUFFER(DSERR_BUFFERLOST);
      return;
    }

    if(!XT_VERIFY_ON_BUFFER(_inputBuffer->Lock(lockPosition, available, &audio1, &size1, &audio2, &size2, 0))) return;
    if(size2 == 0)
    {
      buffer.input = audio1;
      buffer.output = nullptr;
      buffer.frames = available / _frameSize;
      OnBuffer(&buffer);
      if(!XT_VERIFY_ON_BUFFER(_inputBuffer->Unlock(audio1, size1, audio2, size2))) return;
    } else
    {
      XtiDsCombineBufferParts(_audio, audio1, size1, audio2, size2);
      if(!XT_VERIFY_ON_BUFFER(_inputBuffer->Unlock(audio1, size1, audio2, size2))) return;
      buffer.output = nullptr;
      buffer.input = _audio.data();
      buffer.frames = available / _frameSize;
      OnBuffer(&buffer);
    }
    _xtProcessed += available;
  }

  if(_outputBuffer && !prefill)
  {
    if(!XT_VERIFY_ON_BUFFER(_outputBuffer->GetCurrentPosition(&read, &write))) return;
    int32_t gap = XtiDsWrapAround(write - read, bufferBytes);
    _dsProcessed += XtiDsWrapAround(read - _previousPosition, bufferBytes);
    DWORD lockPosition = _xtProcessed % bufferBytes;
    int32_t available = static_cast<int32_t>(bufferBytes - gap - (_xtProcessed - _dsProcessed));
    _previousPosition = read;
    if(available <= 0) return;
    if(XtiDsInsideSafetyGap(read, write, lockPosition)) 
    {
      XT_VERIFY_ON_BUFFER(DSERR_BUFFERLOST);
      return;
    }

    if(!XT_VERIFY_ON_BUFFER(_outputBuffer->Lock(lockPosition, available, &audio1, &size1, &audio2, &size2, 0))) return;
    if(size2 == 0)
    {
      buffer.input = nullptr;
      buffer.output = audio1;
      buffer.frames = available / _frameSize;
      OnBuffer(&buffer);
    } else {
      buffer.input = nullptr;
      buffer.output = _audio.data();
      buffer.frames = available / _frameSize;
      OnBuffer(&buffer);
      XtiDsSplitBufferParts(_audio, audio1, size1, audio2, size2);
    }
    if(!XT_VERIFY_ON_BUFFER(_outputBuffer->Unlock(audio1, size1, audio2, size2))) return;
    _xtProcessed += available;
  }
}

#endif // XT_ENABLE_DSOUND