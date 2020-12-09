#if XT_ENABLE_JACK
#include <xt/jack/Shared.hpp>
#include <xt/jack/Private.hpp>
#include <xt/private/Linux.hpp>
#include <cstring>

void
XtiJackErrorCallback(char const* msg)
{ XT_TRACE(msg); }
void
XtiJackSilentCallback(char const*) { }
std::unique_ptr<XtService>
XtiCreateJackService()
{ return std::make_unique<JackService>(); }

XtServiceError
XtiGetJackError(XtFault fault)
{
  XtServiceError result;
  result.text = strerror(fault);
  result.cause = XtiGetPosixFaultCause(fault);
  return result;
}

int32_t
XtiJackCountPorts(jack_client_t* client, XtBool output)
{
  int32_t count = 0;
  unsigned long flag = output? JackPortIsInput: JackPortIsOutput;
  JackPtr<char const*> ports(jack_get_ports(client, nullptr, JACK_DEFAULT_AUDIO_TYPE, flag));
  while(ports.p[count] != nullptr) count++;
  return count;
}

XtFault
XtiJackCreatePorts(jack_client_t* jc, uint32_t channels, uint64_t mask, unsigned long xtFlag, unsigned long jackFlag, const char* name, std::vector<XtJackPort>& result) {

  const char* type = JACK_DEFAULT_AUDIO_TYPE;
  for(int32_t i = 0; i < channels; i++) {
    std::ostringstream oss;
    oss << name << (i + 1);
    unsigned long flags = xtFlag | JackPortIsTerminal;
    jack_port_t* port = jack_port_register(client, oss.str().c_str(), type, flags, 0);
    if(port == nullptr)
      return ENOENT;
    result.emplace_back(XtJackPort(client, port));
  }

  JackPtr<const char*> jackPorts(jack_get_ports(client, nullptr, type, jackFlag));
  if(mask == 0)
    for(int32_t i = 0; i < channels; i++)
      result[i].connectTo = jackPorts.p[i];
  else
    for(int32_t i = 0, j = 0; i < 64; i++)
      if(mask & (1ULL << i))
        result[j++].connectTo = jackPorts.p[i];
  return 0;
}

#endif // XT_ENABLE_JACK