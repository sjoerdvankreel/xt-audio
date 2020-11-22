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
  XtOnError onError;
  std::thread::id threadId;
  std::vector<std::unique_ptr<XtService>> services;
  static XtPlatform* instance;
};

inline XtPlatform*
XtPlatform::instance = nullptr;

#endif // XT_PRIVATE_PLATFORM_HPP