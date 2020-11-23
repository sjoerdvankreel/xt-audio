#include <xt/private/OS.hpp>
#ifdef __linux__

void
XtiPlatformInit() {}
void
XtiPlatformDestroy() {}
void
XtiDestroyMessageWindow(void* window) {}
void* 
XtiCreateMessageWindow()
{ return nullptr; }

#endif // __linux__