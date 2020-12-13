#if XT_ENABLE_DSOUND
#include <xt/api/private/Platform.hpp>
#include <xt/api/private/Service.hpp>
#include <xt/api/private/Device.hpp>
#include <xt/api/private/Stream.hpp>
#include <xt/private/BlockingStream.hpp>
#include <xt/private/Shared.hpp>
#include <xt/private/Win32.hpp>
#include <xt/private/Services.hpp>
#define INITGUID 1
#include <dsound.h>
#include <mmdeviceapi.h>
#include <audioclient.h>
#include <vector>
#include <memory>

// ---- local ----




// ---- forward ----

// ---- local ----

// ---- service ----

// ---- device ----

// ---- stream ----

void DSoundStream::ProcessBuffer(bool prefill) {
  
  void* audio1;
  void* audio2;
  int32_t gap, available, dsProcessed;
  DWORD bufferBytes = static_cast<DWORD>(buffer.size());
  DWORD size1, size2, read, write, lockPosition, waitResult;
  DWORD bufferMillis = static_cast<DWORD>(bufferFrames * 1000.0 / _params.format.mix.rate);

  XtBuffer xtBuffer = { 0 };

  if(!prefill && !_secondary) {
    waitResult = WaitForSingleObject(timer.timer, bufferMillis);
    if(waitResult == WAIT_TIMEOUT)
      return;
    XT_ASSERT(waitResult == WAIT_OBJECT_0);
  }
  
  if(prefill && render) {
    if(!XT_VERIFY_ON_BUFFER(render->Lock(0, bufferBytes, &audio1, &size1, &audio2, &size2, 0)))
      return;
    if(size2 == 0) {
      xtBuffer.input = nullptr;
      xtBuffer.output = audio1;
      xtBuffer.frames = bufferFrames;
      OnBuffer(&xtBuffer);
    } else {
      xtBuffer.input = nullptr;
      xtBuffer.output = &buffer[0];
      xtBuffer.frames = bufferFrames;
      OnBuffer(&xtBuffer);
      SplitBufferParts(buffer, audio1, size1, audio2, size2);
    }
    if(!XT_VERIFY_ON_BUFFER(render->Unlock(audio1, size1, audio2, size2)))
      return;
    xtBytesProcessed += bufferBytes;
    return;
  }

  if(capture && !prefill) {
    if(!XT_VERIFY_ON_BUFFER(capture->GetCurrentPosition(&write, &read)))
      return;
    gap = WrapAround(write - read, bufferBytes);
    dsProcessed = WrapAround(write - previousDsPosition, bufferBytes);
    dsBytesProcessed += dsProcessed;
    lockPosition = xtBytesProcessed % bufferBytes;
    available = static_cast<int32_t>(dsBytesProcessed - xtBytesProcessed - gap);
    previousDsPosition = write;
    if(available <= 0)
      return;
    if(InsideSafetyGap(read, write, lockPosition)) {
      XT_VERIFY_ON_BUFFER(DSERR_BUFFERLOST);
      return;
    }
    if(!XT_VERIFY_ON_BUFFER(capture->Lock(lockPosition, available, &audio1, &size1, &audio2, &size2, 0)))
      return;
    if(size2 == 0) {
      xtBuffer.input = audio1;
      xtBuffer.output = nullptr;
      xtBuffer.frames = available / frameSize;
      OnBuffer(&xtBuffer);
      if(!XT_VERIFY_ON_BUFFER(capture->Unlock(audio1, size1, audio2, size2)))
        return;
    } else {
      CombineBufferParts(buffer, audio1, size1, audio2, size2);
      if(!XT_VERIFY_ON_BUFFER(capture->Unlock(audio1, size1, audio2, size2)))
        return;
      xtBuffer.input = &buffer[0];
      xtBuffer.output = nullptr;
      xtBuffer.frames = available / frameSize;
      OnBuffer(&xtBuffer);
    }
    xtBytesProcessed += available;
  }

  if(render && !prefill) {
    if(!XT_VERIFY_ON_BUFFER(render->GetCurrentPosition(&read, &write)))
      return;
    gap = WrapAround(write - read, bufferBytes);
    dsProcessed = WrapAround(read - previousDsPosition, bufferBytes);
    dsBytesProcessed += dsProcessed;
    lockPosition = xtBytesProcessed % bufferBytes;
    available = static_cast<int32_t>(bufferBytes - gap - (xtBytesProcessed - dsBytesProcessed));
    previousDsPosition = read;
    if(available <= 0)
      return;
    if(InsideSafetyGap(read, write, lockPosition)) {
      XT_VERIFY_ON_BUFFER(DSERR_BUFFERLOST);
      return;
    }
    if(!XT_VERIFY_ON_BUFFER(render->Lock(lockPosition, available, &audio1, &size1, &audio2, &size2, 0)))
      return;
    if(size2 == 0) {
      xtBuffer.input = nullptr;
      xtBuffer.output = audio1;
      xtBuffer.frames = available / frameSize;
      OnBuffer(&xtBuffer);
    } else {
      xtBuffer.input = nullptr;
      xtBuffer.output = &buffer[0];
      xtBuffer.frames = available / frameSize;
      OnBuffer(&xtBuffer);
      SplitBufferParts(buffer, audio1, size1, audio2, size2);
    }
    if(!XT_VERIFY_ON_BUFFER(render->Unlock(audio1, size1, audio2, size2)))
      return;
    xtBytesProcessed += available;
  }
}

#endif // XT_ENABLE_DSOUND