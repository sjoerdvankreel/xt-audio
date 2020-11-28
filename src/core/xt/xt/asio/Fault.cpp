#if XT_ENABLE_ASIO
#include <xt/asio/Fault.hpp>
#include <Windows.h>
#include <common/iasiodrv.h>
#include <host/pc/asiolist.h>

XtCause
XtiGetAsioFaultCause(XtFault fault)
{
  switch(fault) 
  {
  case ASE_NoClock:
  case ASE_NotPresent:
  case DRVERR_DEVICE_NOT_FOUND:
  case DRVERR_DEVICE_ALREADY_OPEN: return XtCauseEndpoint;
  case XT_ASE_Format: return XtCauseFormat;
  default: return XtCauseUnknown;
  }
}

char const* 
XtiGetAsioFaultText(XtFault fault)
{
  switch(fault) 
  {

  case ASE_OK: return XT_STRINGIFY(ASE_OK);
  case ASE_SUCCESS: return XT_STRINGIFY(ASE_SUCCESS);
  case ASE_NoClock: return XT_STRINGIFY(ASE_NoClock);
  case ASE_NoMemory: return XT_STRINGIFY(ASE_NoMemory);
  case ASE_NotPresent: return XT_STRINGIFY(ASE_NotPresent);
  case ASE_InvalidMode: return XT_STRINGIFY(ASE_InvalidMode);
  case ASE_HWMalfunction: return XT_STRINGIFY(ASE_HWMalfunction);
  case ASE_SPNotAdvancing: return XT_STRINGIFY(ASE_SPNotAdvancing);
  case ASE_InvalidParameter: return XT_STRINGIFY(ASE_InvalidParameter);
  case XT_ASE_Format: return XT_STRINGIFY(XT_ASE_Format);
  case DRVERR_INVALID_PARAM: return XT_STRINGIFY(DRVERR_INVALID_PARAM);
  case DRVERR_DEVICE_NOT_FOUND: return XT_STRINGIFY(DRVERR_DEVICE_NOT_FOUND);
  case DRVERR_DEVICE_ALREADY_OPEN: return XT_STRINGIFY(DRVERR_DEVICE_ALREADY_OPEN);
  default: return "Unknown fault.";
  }
}

#endif // XT_ENABLE_ASIO