#ifndef XT_PRIVATE_OS_HPP
#define XT_PRIVATE_OS_HPP

#include <xt/audio/Enums.h>

void
XtiPlatformInit();
void
XtiPlatformDestroy();
void*
XtiCreateMessageWindow();
XtSystem
XtiSetupToSystem(XtSetup setup);
void
XtiDestroyMessageWindow(void* window);

#endif // XT_PRIVATE_OS_HPP