#ifndef XT_PRIVATE_PLATFORM_HPP
#define XT_PRIVATE_PLATFORM_HPP

#include <xt/api/Callbacks.h>
#include <xt/private/Service.hpp>

#include <string>
#include <vector>
#include <memory>
#include <thread>

struct XtPlatform
{
  ~XtPlatform();
  XtPlatform() = default;
  static inline XtPlatform* instance = nullptr;

  bool Init(void* window);
  XtSystem SetupToSystem(XtSetup setup) const;
  XtService const* GetService(XtSystem system) const;
  void GetSystems(XtSystem* buffer, int32_t* size) const;

  void* _window;
  bool _ownWindow;
  std::string _id;
  std::thread::id _threadId;
  std::vector<std::unique_ptr<XtService>> _services;

  static void EndThread(); 
  static void BeginThread();
  static void RevertThreadPriority(int32_t policy, int32_t previous);
  static void RaiseThreadPriority(int32_t* policy, int32_t* previous);
};

#endif // XT_PRIVATE_PLATFORM_HPP