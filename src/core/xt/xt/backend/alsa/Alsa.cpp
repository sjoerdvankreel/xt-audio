#if XT_ENABLE_ALSA
#include <xt/shared/Linux.hpp>
#include <xt/backend/alsa/Shared.hpp>
#include <xt/backend/alsa/Private.hpp>

#include <memory>
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

std::string
XtiGetAlsaDeviceId(XtAlsaDeviceInfo const& info)
{
  std::ostringstream sstream;
  sstream << "{" << info.name.c_str() << "}";
  sstream << ".{" << static_cast<int32_t>(info.type) << "}";
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

#endif // XT_ENABLE_ALSA