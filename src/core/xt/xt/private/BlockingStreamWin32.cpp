#include <xt/private/BlockingStream.hpp>
#ifdef _WIN32
#include <Windows.h>

void
XtBlockingStream::EndCallbackThread()
{ CoUninitialize(); }
void
XtBlockingStream::BeginCallbackThread()
{ XT_ASSERT(SUCCEEDED(CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED))); }

#endif // _WIN32