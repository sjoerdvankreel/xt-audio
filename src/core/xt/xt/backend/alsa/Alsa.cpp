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

std::string
XtiGetAlsaDeviceId(XtAlsaDeviceInfo const& info)
{
  std::ostringstream sstream;
  sstream << info.name.c_str() << ",TYPE=";
  sstream << static_cast<int32_t>(info.type);
  return sstream.str();
}

XtServiceError
XtiGetAlsaError(XtFault fault)
{
  XtServiceError result;
  result.text = snd_strerror(fault);
  result.cause = XtiGetPosixFaultCause(std::abs(static_cast<int>(fault)));
  return result;
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

#endif // XT_ENABLE_ALSA