#ifndef XT_DSOUND_PRIVATE_HPP
#define XT_DSOUND_PRIVATE_HPP
#if XT_ENABLE_DSOUND

#include <xt/api/public/Enums.h>
#include <xt/private/Shared.hpp>
#include <string>

struct XtDSDeviceInfo
{
  std::string id;
  std::string name;
};

char const* 
XtiGetDSoundFaultText(XtFault fault);
XtCause 
XtiGetDSoundFaultCause(XtFault fault);

#endif // XT_ENABLE_DSOUND
#endif // XT_DSOUND_PRIVATE_HPP