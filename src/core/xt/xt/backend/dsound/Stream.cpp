#if XT_ENABLE_DSOUND
#include <xt/backend/dsound/Shared.hpp>
#include <xt/backend/dsound/Private.hpp>

XtFault
DSoundStream::GetFrames(int32_t* frames) const 
{ *frames = _bufferFrames; return S_OK; }
XtFault
DSoundStream::GetLatency(XtLatency* latency) const
{ return S_OK; }
void*
DSoundStream::GetHandle()
{ return _inputBuffer.p != nullptr? _inputBuffer.p: static_cast<void*>(_outputBuffer.p); }

void
DSoundStream::StopSlaveBuffer()
{
  if(_inputBuffer) XT_TRACE_IF(FAILED(_inputBuffer->Stop()));
  else XT_TRACE_IF(FAILED(_outputBuffer->Stop()));
  _xtProcessed = 0;
  _dsProcessed = 0;
  _previousPosition = 0;
}

XtFault
DSoundStream::StartSlaveBuffer()
{
  HRESULT hr;
  memset(_audio.data(), 0, _audio.size());
  if(_inputBuffer) XT_VERIFY_COM(_inputBuffer->Start(DSCBSTART_LOOPING));
  else XT_VERIFY_COM(_outputBuffer->Play(0, 0, DSBPLAY_LOOPING));
  return DS_OK;
}

void
DSoundStream::StopMasterBuffer()
{
  auto rate = _params.format.mix.rate;
  UINT period = XtiDsGetTimerPeriod(_bufferFrames, rate);
  XT_TRACE_IF(!CancelWaitableTimer(_timer.timer));
  XT_TRACE_IF(timeEndPeriod(period / 2) != TIMERR_NOERROR);
  XT_TRACE_IF(!SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_NORMAL));
}

XtFault
DSoundStream::BlockMasterBuffer()
{
  auto rate = _params.format.mix.rate;
  DWORD bufferMillis = static_cast<DWORD>(_bufferFrames * 1000.0 / rate);
  XT_VERIFY(WaitForSingleObject(_timer.timer, bufferMillis) == WAIT_OBJECT_0, DSERR_GENERIC);
  return DS_OK;
}

XtFault
DSoundStream::StartMasterBuffer()
{
  LARGE_INTEGER due;
  due.QuadPart = -1;
  auto rate = _params.format.mix.rate;
  UINT period = XtiDsGetTimerPeriod(_bufferFrames, rate);
  XT_VERIFY(timeBeginPeriod(period / 2) == TIMERR_NOERROR, DSERR_GENERIC);
  auto timeGuard = XtiGuard([this] { XT_ASSERT(CancelWaitableTimer(_timer.timer)); } );
  XT_VERIFY(SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL), DSERR_GENERIC);
  auto prioGuard = XtiGuard([] { XT_ASSERT(SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_NORMAL)); });
  XT_VERIFY(SetWaitableTimer(_timer.timer, &due, period, nullptr, nullptr, TRUE), DSERR_GENERIC);
  timeGuard.Commit();
  prioGuard.Commit();
  return DS_OK;
}

XtFault
DSoundStream::PrefillOutputBuffer()
{
  HRESULT hr;
  void* audio1;
  void* audio2;
  DWORD size1, size2;
  XtBuffer buffer = { 0 };
  DWORD bufferBytes = static_cast<DWORD>(_audio.size());

  if(!_outputBuffer) return DS_OK;
  XT_VERIFY_COM(_outputBuffer->Lock(0, bufferBytes, &audio1, &size1, &audio2, &size2, 0));
  if(size2 == 0)
  {
    buffer.output = audio1;
    buffer.frames = _bufferFrames;
    XT_VERIFY_COM(OnBuffer(_params.index, &buffer));
  } else
  {
    buffer.output = _audio.data();
    buffer.frames = _bufferFrames;
    XT_VERIFY_COM(OnBuffer(_params.index, &buffer));
    XtiDsSplitBufferParts(_audio, audio1, size1, audio2, size2);
  }
  XT_VERIFY_COM(_outputBuffer->Unlock(audio1, size1, audio2, size2));
  _xtProcessed += bufferBytes;
  return DS_OK;
}  

XtFault 
DSoundStream::ProcessBuffer()
{  
  HRESULT hr;
  void* audio1;
  void* audio2;
  XtBuffer buffer = { 0 };
  DWORD size1, size2, read, write;
  DWORD bufferBytes = static_cast<DWORD>(_audio.size());

  if(_inputBuffer)
  {
    XT_VERIFY_COM(_inputBuffer->GetCurrentPosition(&write, &read));
    int32_t gap = XtiDsWrapAround(write - read, bufferBytes);
    _dsProcessed += XtiDsWrapAround(write - _previousPosition, bufferBytes);
    if(_xtProcessed > _dsProcessed)
    {
      OnXRun(_params.index);
      _xtProcessed = _dsProcessed - gap;
    }
    DWORD lockPosition = _xtProcessed % bufferBytes;
    int32_t available = static_cast<int32_t>(_dsProcessed - _xtProcessed - gap);
    _previousPosition = write;
    if(available <= 0) return DS_OK;
    if(XtiDsInsideSafetyGap(read, write, lockPosition))
    {
      XT_VERIFY_COM(DSERR_BUFFERLOST);
      return DS_OK;
    }

    XT_VERIFY_COM(_inputBuffer->Lock(lockPosition, available, &audio1, &size1, &audio2, &size2, 0));
    if(size2 == 0)
    {
      buffer.input = audio1;
      buffer.frames = available / _frameSize;
      XT_VERIFY_COM(OnBuffer(_params.index, &buffer));
      XT_VERIFY_COM(_inputBuffer->Unlock(audio1, size1, audio2, size2));
    } else
    {
      XtiDsCombineBufferParts(_audio, audio1, size1, audio2, size2);
      XT_VERIFY_COM(_inputBuffer->Unlock(audio1, size1, audio2, size2));
      buffer.input = _audio.data();
      buffer.frames = available / _frameSize;
      XT_VERIFY_COM(OnBuffer(_params.index, &buffer));
    }
    _xtProcessed += available;
    return DS_OK;
  }

  XT_VERIFY_COM(_outputBuffer->GetCurrentPosition(&read, &write));
  int32_t gap = XtiDsWrapAround(write - read, bufferBytes);
  _dsProcessed += XtiDsWrapAround(read - _previousPosition, bufferBytes);
  if(_xtProcessed < _dsProcessed)
  {
    OnXRun(_params.index);
    _xtProcessed = _dsProcessed + gap;
  }
  DWORD lockPosition = _xtProcessed % bufferBytes;
  int32_t available = static_cast<int32_t>(bufferBytes - gap - (_xtProcessed - _dsProcessed));
  _previousPosition = read;
  if(available <= 0) return DS_OK;
  if(XtiDsInsideSafetyGap(read, write, lockPosition)) 
  {
    XT_VERIFY_COM(DSERR_BUFFERLOST);
    return DS_OK;
  }

  XT_VERIFY_COM(_outputBuffer->Lock(lockPosition, available, &audio1, &size1, &audio2, &size2, 0));
  if(size2 == 0)
  {
    buffer.output = audio1;
    buffer.frames = available / _frameSize;
    XT_VERIFY_COM(OnBuffer(_params.index, &buffer));
  } else {
    buffer.output = _audio.data();
    buffer.frames = available / _frameSize;
    XT_VERIFY_COM(OnBuffer(_params.index, &buffer));
    XtiDsSplitBufferParts(_audio, audio1, size1, audio2, size2);
  }
  XT_VERIFY_COM(_outputBuffer->Unlock(audio1, size1, audio2, size2));
  _xtProcessed += available;
  return DS_OK;
}

#endif // XT_ENABLE_DSOUND