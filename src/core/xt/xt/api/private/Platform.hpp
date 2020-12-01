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

  static inline XtPlatform* instance = nullptr;
  static XtSystem SetupToSystem(XtSetup setup);

  void* _window;
  bool _ownWindow;
  std::string _id;
  XtOnError _onError;
  std::thread::id _threadId;
  std::vector<std::unique_ptr<XtService>> _services;
};

#endif // XT_API_PRIVATE_PLATFORM_HPP