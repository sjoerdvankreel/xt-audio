#ifndef XT_WIN32_HPP
#define XT_WIN32_HPP
#ifdef _WIN32

#include <xt/api/public/Structs.h>
#define NOMINMAX 1
#include <windows.h>
#include <atlbase.h>
#include <mmsystem.h>
#include <mmreg.h>
#include <ks.h>
#include <ksmedia.h>
#include <string>

// ---- win32 ----

#define XT_VERIFY_COM(e)     \
do {                         \
  if(FAILED(hr = (e)))       \
    return XT_TRACE(#e), hr; \
} while(0)

extern const char* XtwWfxChannelNames[18];

std::string XtwWideStringToUtf8(const wchar_t* wide);
bool XtwFormatToWfx(const XtFormat& format, WAVEFORMATEXTENSIBLE& wfx);
bool XtwWfxToFormat(const WAVEFORMATEX& wfx, XtBool output, XtFormat& format);

#endif // _WIN32
#endif // XT_WIN32_HPP