#ifndef XT_BACKEND_ALSA_PRIVATE_HPP
#define XT_BACKEND_ALSA_PRIVATE_HPP
#if XT_ENABLE_ALSA

#include <alsa/asoundlib.h>
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

struct XtAlsaPcm
{
  snd_pcm_t* pcm;
  snd_pcm_hw_params_t* params;

  ~XtAlsaPcm();
  XtAlsaPcm() = default;
  XtAlsaPcm(XtAlsaPcm const&) = delete;
  XtAlsaPcm& operator=(XtAlsaPcm const&) = delete;
};

snd_pcm_format_t
XtiToAlsaSample(XtSample sample);
bool
XtiAlsaTypeIsMMap(XtAlsaType type);
bool
XtiAlsaTypeIsOutput(XtAlsaType type);
char const*
XtiGetAlsaNameSuffix(XtAlsaType type);
std::string
XtiGetAlsaHint(void const* hint, char const* id);
std::string
XtiGetAlsaDeviceId(XtAlsaDeviceInfo const& info);
snd_pcm_access_t
XtiGetAlsaAccess(XtAlsaType type, XtBool interleaved);
int
XtiAlsaOpenPcm(XtAlsaDeviceInfo const& info, XtAlsaPcm* pcm);
bool
XtiParseAlsaDeviceInfo(std::string const& id, XtAlsaDeviceInfo* info);
int
XtiAlsaOpenPcm(XtAlsaDeviceInfo const& info, XtFormat const* format, XtAlsaPcm* pcm);
void
XtiLogAlsaError(char const* file, int line, char const* fun, int err, char const* fmt, ...);

inline uint8_t*
XtiGetAlsaMMapAddress(snd_pcm_channel_area_t const* areas, size_t index, snd_pcm_uframes_t offset)
{ return static_cast<uint8_t*>(areas[index].addr) + areas[index].first / 8 + offset * areas[index].step / 8; }

#endif // XT_ENABLE_ALSA
#endif // XT_BACKEND_ALSA_PRIVATE_HPP