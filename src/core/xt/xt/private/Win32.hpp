#ifndef XT_PRIVATE_WIN32_HPP
#define XT_PRIVATE_WIN32_HPP
#ifdef _WIN32
#define NOMINMAX 1
#include <xt/api/public/Shared.h>
#include <xt/api/public/Structs.h>
#include <Windows.h>
#include <atlbase.h>
#include <mmreg.h>
#include <ks.h>
#include <ksmedia.h>
#include <string>

extern char const*
XtiWfxChannelNames[18];

#define XT_VERIFY_COM(e) \
  do { if(FAILED(hr = (e))) return XT_TRACE(#e), hr; } while(0)

struct XtPropVariant
{
  PROPVARIANT pv;
  ~XtPropVariant() { PropVariantClear(&pv); }
  XtPropVariant(): pv() { PropVariantInit(&pv); }
  XtPropVariant(XtPropVariant const&) = delete;
  XtPropVariant& operator=(XtPropVariant const&) = delete;
};

CLSID
XtiUtf8ToClassId(char const* utf8);
std::wstring
XtiUtf8ToWideString(char const* utf8);
std::string
XtiClassIdToUtf8(CLSID const& classId);
std::string
XtiWideStringToUtf8(wchar_t const* wide);
bool
XtiFormatToWfx(XtFormat const& format, WAVEFORMATEXTENSIBLE& wfx);
bool
XtiWfxToFormat(WAVEFORMATEX const& wfx, XtBool output, XtFormat& format);

#endif // _WIN32
#endif // XT_PRIVATE_WIN32_HPP