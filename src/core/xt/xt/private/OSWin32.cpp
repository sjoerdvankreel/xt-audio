#include <xt/private/OS.hpp>
#ifdef _WIN32
#include <Windows.h>
#include <xt/Private.hpp>
#include <cassert>

void
XtiPlatformDestroy()
{ CoUninitialize(); }
void 
XtiDestroyMessageWindow(void* window)
{ DestroyWindow(static_cast<HWND>(window)); }
void
XtiPlatformInit()
{ XT_ASSERT(SUCCEEDED(CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED))); }

XtSystem
XtiSetupToSystem(XtSetup setup)
{
  switch(setup)
  {
  case XtSetupProAudio: return XtSystemASIO;
  case XtSetupSystemAudio: return XtSystemWASAPI;
  case XtSetupConsumerAudio: return XtSystemDSound;
  default: assert(false); return static_cast<XtSystem>(0);
  }
}

void* 
XtiCreateMessageWindow()
{ 
  HWND result;
  XT_ASSERT(result = CreateWindow("STATIC", 0, 0, 0, 0, 0, 0, HWND_MESSAGE, 0, 0, 0));
  return result;
}

#endif // _WIN32