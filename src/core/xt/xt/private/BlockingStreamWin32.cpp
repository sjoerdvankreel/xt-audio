#include <xt/private/BlockingStreamWin32.hpp>
#ifdef _WIN32





DWORD WINAPI 
XtWin32BlockingStream::OnBlockingBuffer(void* user)
{
  //XtBlockingStreamState state;
  //auto stream = static_cast<XtBlockingStream*>(user);
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
    {
      
    }  
    
      XT_ASSERT(pthread_mutex_lock(&stream->self().lock.m) == 0);
      XT_ASSERT(pthread_cond_wait(&stream->self().control.cv, &stream->self().lock.m) == 0);
      XT_ASSERT(pthread_mutex_unlock(&stream->self().lock.m) == 0);
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



static void* LinuxOnBlockingBuffer(void* user) {

  int policy;
  int maxPriority;
  int startPriority;
  struct sched_param param;
  //XtBlockingStreamState state;
  //auto stream = static_cast<XtBlockingStream*>(user);

  XT_ASSERT(pthread_getschedparam(pthread_self(), &policy, &param) == 0);
  startPriority = param.sched_priority;
  maxPriority = sched_get_priority_max(policy);
  while((state = ReadLinuxBlockingStreamState(stream)) != XtBlockingStreamState::Closed) {
    switch(state) {
    case XtBlockingStreamState::Started:
      stream->ProcessBuffer(false);
      break;
    case XtBlockingStreamState::Closing:
      ReceiveLinuxBlockingStreamControl(stream, XtBlockingStreamState::Closed);
      return nullptr;
    case XtBlockingStreamState::Stopping:
      stream->StopStream();
      param.sched_priority = startPriority;
      XT_ASSERT(pthread_setschedparam(pthread_self(), policy, &param) == 0);
      ReceiveLinuxBlockingStreamControl(stream, XtBlockingStreamState::Stopped);
      break;
    case XtBlockingStreamState::Starting:
      stream->ProcessBuffer(true);
      param.sched_priority = maxPriority;
      XT_ASSERT(pthread_setschedparam(pthread_self(), policy, &param) == 0);
      stream->StartStream();
      ReceiveLinuxBlockingStreamControl(stream, XtBlockingStreamState::Started);
      break;
    case XtBlockingStreamState::Stopped:
      XT_ASSERT(pthread_mutex_lock(&stream->self().lock.m) == 0);
      XT_ASSERT(pthread_cond_wait(&stream->self().control.cv, &stream->self().lock.m) == 0);
      XT_ASSERT(pthread_mutex_unlock(&stream->self().lock.m) == 0);
      break;
    default:
      XT_FAIL("Unexpected stream state.");
      break;
    }
  }
  XT_FAIL("End of stream callback reached.");
  return nullptr;
}

#endif // _WIN32