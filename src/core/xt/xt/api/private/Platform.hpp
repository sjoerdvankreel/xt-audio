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

  void* _window;
  bool _ownWindow;
  std::string _id;
  XtOnError _onError;
  std::thread::id _threadId;
  std::vector<std::unique_ptr<XtService>> _services;

  static void EndThread(); 
  static void BeginThread();
  static XtSystem SetupToSystem(XtSetup setup);
  static XtCause GetPosixFaultCause(XtFault fault);
  static void RevertThreadPriority(int32_t policy, int32_t previous);
  static void RaiseThreadPriority(int32_t* policy, int32_t* previous);
};

#endif // XT_API_PRIVATE_PLATFORM_HPP