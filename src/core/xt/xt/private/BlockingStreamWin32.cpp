#include <xt/private/BlockingStreamWin32.hpp>
#ifdef _WIN32

XtBlockingStreamState
XtBlockingStreamBase<XtWin32BlockingStream>::
ReadState()
{
  XtBlockingStreamState result;
  EnterCriticalSection(&self().lock.cs);
  result = _state;
  LeaveCriticalSection(&self().lock.cs);
  return result;
}

void
XtBlockingStreamBase<XtWin32BlockingStream>::
RequestStop() 
{
  StopStream();
  if(_secondary) return;
  EnterCriticalSection(&self().lock.cs);
  _state = XtBlockingStreamState::Stopped;
  XT_ASSERT(SetEvent(self().respond.event));
  LeaveCriticalSection(&self().lock.cs);
}

void
XtBlockingStreamBase<XtWin32BlockingStream>::
ReceiveControl(XtBlockingStreamState state)
{
  EnterCriticalSection(&self().lock.cs);
  _state = state;
  XT_ASSERT(SetEvent(self().respond.event));
  LeaveCriticalSection(&self().lock.cs);
}

XtWin32BlockingStream::
XtWin32BlockingStream(bool secondary):
XtBlockingStreamBase(secondary), respond(), control(), lock()
{
  if(secondary) return;
  HANDLE thread = CreateThread(nullptr, 0, &OnBlockingBuffer, this, 0, nullptr);
  XT_ASSERT(thread != nullptr);
  CloseHandle(thread);
}

void
XtBlockingStreamBase<XtWin32BlockingStream>::
SendControl(XtBlockingStreamState from, XtBlockingStreamState to)
{
  EnterCriticalSection(&self().lock.cs);
  if(_state == to) { LeaveCriticalSection(&self().lock.cs); return; }
  _state = from;
  XT_ASSERT(SetEvent(self().control.event));
  LeaveCriticalSection(&self().lock.cs);
  while(ReadState() != to) 
    XT_ASSERT(WaitForSingleObject(self().respond.event, XT_WAIT_TIMEOUT_MS) == WAIT_OBJECT_0);
}

DWORD WINAPI 
XtWin32BlockingStream::OnBlockingBuffer(void* user)
{
  XtBlockingStreamState state;
  auto stream = static_cast<XtBlockingStream*>(user);
  XT_ASSERT(SUCCEEDED(CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED)));
  while((state = stream->ReadState()) != XtBlockingStreamState::Closed)
  {
    switch(state)
    {
    case XtBlockingStreamState::Started:
      stream->ProcessBuffer(false);
      break;
    case XtBlockingStreamState::Closing:
      stream->ReceiveControl(XtBlockingStreamState::Closed);
      CoUninitialize();
      return S_OK;
    case XtBlockingStreamState::Stopping:
      stream->StopStream();
      stream->ReceiveControl(XtBlockingStreamState::Stopped);
      break;
    case XtBlockingStreamState::Starting:
      stream->ProcessBuffer(true);
      stream->StartStream();
      stream->ReceiveControl(XtBlockingStreamState::Started);
      break;
    case XtBlockingStreamState::Stopped:
      XT_ASSERT(WaitForSingleObject(stream->self().control.event, INFINITE) == WAIT_OBJECT_0);
      break;
    default:
      XT_FAIL("Unexpected stream state.");
      break;
    }
  }
  XT_FAIL("End of stream callback reached.");
  return S_OK;
}

#endif // _WIN32