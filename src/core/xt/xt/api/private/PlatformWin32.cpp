#include <xt/api/private/Platform.hpp>
#ifdef _WIN32
#include <xt/Private.hpp>
#include <Windows.h>

XtPlatform::~XtPlatform()
{
  if(_ownWindow) DestroyWindow(static_cast<HWND>(_window));
  CoUninitialize();
}

XtSystem
XtPlatform::SetupToSystem(XtSetup setup)
{
  switch(setup)
  {
  case XtSetupProAudio: return XtSystemASIO;
  case XtSetupSystemAudio: return XtSystemWASAPI;
  case XtSetupConsumerAudio: return XtSystemDSound;
  default: XT_ASSERT(false); return static_cast<XtSystem>(0);
  }
}

XtPlatform::XtPlatform(void* window):
XtPlatform()
{
  _window = window;
  _ownWindow = window == nullptr;
  XT_ASSERT(SUCCEEDED(CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED)));
  if(window == nullptr) XT_ASSERT(_window = CreateWindow("STATIC", 0, 0, 0, 0, 0, 0, HWND_MESSAGE, 0, 0, 0));
}

#endif // _WIN32