#ifndef XT_PRIVATE_PLATFORM_HPP
#define XT_PRIVATE_PLATFORM_HPP

#include <xt/audio/Callbacks.h>
#include <xt/private/Service.hpp>
#include <string>
#include <vector>
#include <memory>
#include <thread>

struct XtPlatform
{
  ~XtPlatform();
  XtPlatform() = default;
  XtPlatform(void* window);

  void* _window;
  bool _ownWindow;
  std::string _id;
  XtOnError _onError;
  std::thread::id _threadId;
  std::vector<std::unique_ptr<XtService>> _services;

  static XtPlatform* instance;
  static XtSystem SetupToSystem(XtSetup setup);
  static int32_t LockIncr(int32_t volatile* dest);
  static int32_t LockDecr(int32_t volatile* dest);
  static int32_t Cas(int32_t volatile* dest, int32_t exch, int32_t comp);
};

inline XtPlatform*
XtPlatform::instance = nullptr;

#include <xt/private/PlatformLinux.ipp>
#include <xt/private/PlatformWin32.ipp>

#endif // XT_PRIVATE_PLATFORM_HPP