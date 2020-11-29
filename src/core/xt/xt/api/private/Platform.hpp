#ifndef XT_API_PRIVATE_PLATFORM_HPP
#define XT_API_PRIVATE_PLATFORM_HPP

#include <xt/api/public/Callbacks.h>
#include <xt/api/private/Service.hpp>
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

#include <xt/api/private/PlatformLinux.ipp>
#include <xt/api/private/PlatformWin32.ipp>

#endif // XT_API_PRIVATE_PLATFORM_HPP