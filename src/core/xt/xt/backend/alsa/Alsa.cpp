#if XT_ENABLE_ALSA
#include <xt/shared/Linux.hpp>
#include <xt/backend/alsa/Shared.hpp>
#include <xt/backend/alsa/Private.hpp>

#include <memory>
#include <cstring>
#include <sstream>

std::unique_ptr<XtService>
XtiCreateAlsaService()
{ return std::make_unique<AlsaService>(); }

XtAlsaPcm::
~XtAlsaPcm()
{  
  if(pcm != nullptr) XT_TRACE_IF(snd_pcm_close(pcm));
  if(params != nullptr) snd_pcm_hw_params_free(params);
}

bool
XtiAlsaTypeIsMMap(XtAlsaType type)
{
  switch(type)
  {
  case XtAlsaType::InputRw:
  case XtAlsaType::OutputRw: return false;
  case XtAlsaType::InputMMap: 
  case XtAlsaType::OutputMMap: return true;
  default: XT_ASSERT(false); return false;
  }
}

bool
XtiAlsaTypeIsOutput(XtAlsaType type)
{
  switch(type)
  {
  case XtAlsaType::InputRw:
  case XtAlsaType::InputMMap: return false;
  case XtAlsaType::OutputRw: 
  case XtAlsaType::OutputMMap: return true;
  default: XT_ASSERT(false); return false;
  }
}

std::string
XtiGetAlsaDeviceId(XtAlsaDeviceInfo const& info)
{
  std::ostringstream sstream;
  sstream << info.name.c_str() << ",TYPE=";
  sstream << static_cast<int32_t>(info.type);
  return sstream.str();
}

std::string
XtiGetAlsaHint(void const* hint, char const* id)
{
  char* value = snd_device_name_get_hint(hint, id);
  if(value == nullptr) return std::string();
  std::string result(value);
  free(value);
  return result;
}

char const*
XtiGetAlsaNameSuffix(XtAlsaType type)
{
  switch(type)
  {
  case XtAlsaType::InputRw: return "Input R/W";
  case XtAlsaType::InputMMap: return "Input MMap";
  case XtAlsaType::OutputRw: return "Output R/W";
  case XtAlsaType::OutputMMap: return "Output MMap";
  default: return XT_ASSERT(false), nullptr;
  }
}

XtServiceError
XtiGetAlsaError(XtFault fault)
{
  XtServiceError result;
  result.text = snd_strerror(fault);
  result.cause = XtiGetPosixFaultCause(std::abs(static_cast<int>(fault)));
  return result;
}

int
XtiAlsaOpenPcm(XtAlsaDeviceInfo const& info, XtAlsaPcm* pcm)
{
  int err;
  snd_pcm_t* pcmp;
  snd_pcm_hw_params_t* hpp;
  memset(pcm, 0, sizeof(XtAlsaPcm));
  bool output = XtiAlsaTypeIsOutput(info.type);
  auto stream = output? SND_PCM_STREAM_PLAYBACK: SND_PCM_STREAM_CAPTURE;
  if((err = snd_pcm_open(&pcmp, info.name.c_str(), stream, 0)) < 0) return err;
  auto pcmGuard = XtiGuard([pcmp] { XT_TRACE_IF(snd_pcm_close(pcmp)); });
  XT_VERIFY_ALSA(snd_pcm_hw_params_malloc(&hpp));
  auto paramsGuard = XtiGuard([hpp] { snd_pcm_hw_params_free(hpp); });
  if((err = snd_pcm_hw_params_any(pcmp, hpp)) < 0) return err;
  pcm->pcm = pcmp;
  pcm->params = hpp;
  pcmGuard.Commit();
  paramsGuard.Commit();
  return 0;
}

bool
XtiParseAlsaDeviceInfo(std::string const& id, XtAlsaDeviceInfo* info)
{
  if(id.length() < 7) return false;
  if(id.substr(id.length() - 7, 6) != ",TYPE=") return false;
  char typeCode = id[id.length() - 1];
  auto type = static_cast<XtAlsaType>(typeCode - '0');
  if(!(XtAlsaType::InputRw <= type && type <= XtAlsaType::OutputMMap)) return false;
  info->name = id;
  info->type = type;   
  info->name.erase(id.size() - 7, 7);
  return true;
}

void
XtiLogAlsaError(char const* file, int line, char const* fun, int err, char const* fmt, ...)
{
  if(err == 0) return;
  va_list arg;
  va_list argCopy;
  va_start(arg, fmt);
  va_copy(argCopy, arg);
  int size = vsnprintf(nullptr, 0, fmt, arg);
  std::vector<char> message(static_cast<size_t>(size + 1), '\0');
  vsnprintf(&message[0], size + 1, fmt, argCopy);
  XtiTrace({file, fun, line}, message.data());
  va_end(argCopy);
  va_end(arg);
}

snd_pcm_access_t
XtiGetAlsaAccess(XtAlsaType type, XtBool interleaved)
{
  bool mmap = XtiAlsaTypeIsMMap(type);
  if(mmap) return interleaved? SND_PCM_ACCESS_MMAP_INTERLEAVED: SND_PCM_ACCESS_MMAP_NONINTERLEAVED;
  return interleaved? SND_PCM_ACCESS_RW_INTERLEAVED: SND_PCM_ACCESS_RW_NONINTERLEAVED;
}

#endif // XT_ENABLE_ALSA