#ifdef _WIN32
#include <xt/shared/Win32.hpp>
#include <xt/private/Platform.hpp>

void XtPlatform
::EndThread() { CoUninitialize(); }
void XtPlatform::
RevertThreadPriority(int32_t policy, int32_t previous) { }
void XtPlatform::
RaiseThreadPriority(int32_t* policy, int32_t* previous) { }
void 
XtPlatform::BeginThread() 
{ XT_ASSERT_COM(CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED)); }

XtPlatform::
~XtPlatform()
{
  if(_ownWindow) DestroyWindow(static_cast<HWND>(_window));
  EndThread();
}

XtSystem
XtPlatform::SetupToSystem(XtSetup setup) const
{
  switch(setup)
  {
  case XtSetupProAudio: return XtSystemASIO;
  case XtSetupSystemAudio: return XtSystemWASAPI;
  case XtSetupConsumerAudio: return XtSystemDSound;
  default: XT_ASSERT(false); return static_cast<XtSystem>(0);
  }
}

bool
XtPlatform::Init(void* window)
{
  XT_ASSERT_API(SUCCEEDED(CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED)));
  _window = window;
  _ownWindow = window == nullptr;
  if(window != nullptr) return true;
  auto handle = CreateWindow("STATIC", 0, 0, 0, 0, 0, 0, HWND_MESSAGE, 0, 0, 0);
  XT_ASSERT(_window = handle);
  return true;
}

#endif // _WIN32