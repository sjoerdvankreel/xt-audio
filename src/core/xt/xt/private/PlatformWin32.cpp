#include <xt/private/Platform.hpp>
#ifdef _WIN32
#include <Windows.h>
#include <xt/Private.hpp>
#include <cassert>

int32_t 
XtPlatform::LockIncr(int32_t volatile* dest) 
{ 
  auto d = reinterpret_cast<long volatile*>(dest);
  return InterlockedIncrement(d);
}

int32_t 
XtPlatform::LockDecr(int32_t volatile* dest) 
{
  auto d = reinterpret_cast<long volatile*>(dest);
  return InterlockedDecrement(d); 
}

int32_t 
XtPlatform::Cas(int32_t volatile* dest, int32_t exch, int32_t comp)
{ 
  auto d = reinterpret_cast<volatile long*>(dest);
  return InterlockedCompareExchange(d, exch, comp); 
}

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