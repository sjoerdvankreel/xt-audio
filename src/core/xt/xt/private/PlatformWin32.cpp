#include <xt/private/Platform.hpp>
#ifdef _WIN32
#include <Windows.h>
#include <xt/Private.hpp>
#include <cassert>

XtPlatform::~XtPlatform()
{
  if(ownWindow) DestroyWindow(static_cast<HWND>(window));
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
  default: assert(false); return static_cast<XtSystem>(0);
  }
}

XtPlatform::XtPlatform(void* window):
XtPlatform()
{
  this->window = window;
  this->ownWindow = window == nullptr;
  XT_ASSERT(SUCCEEDED(CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED)));
  if(window == nullptr) XT_ASSERT(this->window = CreateWindow("STATIC", 0, 0, 0, 0, 0, 0, HWND_MESSAGE, 0, 0, 0));
}

#endif // _WIN32