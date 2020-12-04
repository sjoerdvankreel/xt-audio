#ifndef XT_PRIVATE_WIN32_HPP
#define XT_PRIVATE_WIN32_HPP
#ifdef _WIN32
#define NOMINMAX 1
#include <xt/api/public/Shared.h>
#include <xt/api/public/Structs.h>
#include <Windows.h>
#include <mmreg.h>
#include <ks.h>
#include <ksmedia.h>
#include <string>

struct XtPropVariant
{
  PROPVARIANT pv;
  ~XtPropVariant() { PropVariantClear(&pv); }
  XtPropVariant(): pv() { PropVariantInit(&pv); }
  XtPropVariant(XtPropVariant const&) = delete;
  XtPropVariant& operator=(XtPropVariant const&) = delete;
};

std::string
XtiWideStringToUtf8(wchar_t const* wide);
bool
XtiFormatToWfx(XtFormat const& format, WAVEFORMATEXTENSIBLE& wfx);
bool
XtiWfxToFormat(WAVEFORMATEX const& wfx, XtBool output, XtFormat& format);

#endif // _WIN32
#endif // XT_PRIVATE_WIN32_HPP