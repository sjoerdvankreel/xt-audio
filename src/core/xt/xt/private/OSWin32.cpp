#include <xt/private/OS.hpp>
#ifdef _WIN32
#include <Windows.h>
#include <xt/Private.hpp>

void
XtiPlatformDestroy()
{ CoUninitialize(); }
void 
XtiDestroyMessageWindow(void* window)
{ DestroyWindow(static_cast<HWND>(window)); }
void
XtiPlatformInit()
{ XT_ASSERT(SUCCEEDED(CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED))); }

void* 
XtiCreateMessageWindow()
{ 
  HWND result;
  XT_ASSERT(result = CreateWindow("STATIC", 0, 0, 0, 0, 0, 0, HWND_MESSAGE, 0, 0, 0));
  return result;
}

#endif // _WIN32