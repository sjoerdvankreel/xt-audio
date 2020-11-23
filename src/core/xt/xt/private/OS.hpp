#ifndef XT_PRIVATE_OS_HPP
#define XT_PRIVATE_OS_HPP

void
XtiPlatformInit();
void
XtiPlatformDestroy();
void*
XtiCreateMessageWindow();
void
XtiDestroyMessageWindow(void* window);

#endif // XT_PRIVATE_OS_HPP