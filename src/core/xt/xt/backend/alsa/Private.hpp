#ifndef XT_BACKEND_ALSA_PRIVATE_HPP
#define XT_BACKEND_ALSA_PRIVATE_HPP
#if XT_ENABLE_ALSA

#include <string>

#define XT_VERIFY_ALSA(c)     \
  do { int e = (c); if(e < 0) \
  return XT_TRACE(#c), e; } while(0)

enum XtAlsaType
{
  InputRw,
  InputMMap,
  OutputRw,
  OutputMMap
};

struct XtAlsaDeviceInfo
{
  XtAlsaType type;
  std::string name;
};

char const*
XtiGetAlsaNameSuffix(XtAlsaType type);
std::string
XtiGetAlsaHint(void const* hint, char const* id);
std::string
XtiGetAlsaDeviceId(XtAlsaDeviceInfo const& info);
bool
XtiParseAlsaDeviceInfo(std::string const& id, XtAlsaDeviceInfo* info);

#endif // XT_ENABLE_ALSA
#endif // XT_BACKEND_ALSA_PRIVATE_HPP