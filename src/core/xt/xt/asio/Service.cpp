#if XT_ENABLE_ASIO
#include <xt/private/Services.hpp>
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
  case ASE_NoClock: return "ASE_NoClock";
  case ASE_NoMemory: return "ASE_NoMemory";
  case ASE_NotPresent: return "ASE_NotPresent";
  case ASE_InvalidMode: return "ASE_InvalidMode";
  case ASE_HWMalfunction: return "ASE_HWMalfunction";
  case ASE_SPNotAdvancing: return "ASE_SPNotAdvancing";
  case ASE_InvalidParameter: return "ASE_InvalidParameter";
  case XT_ASE_Format: return "XT_ASE_Format";
  case DRVERR_INVALID_PARAM: return "DRVERR_INVALID_PARAM";
  case DRVERR_DEVICE_NOT_FOUND: return "DRVERR_DEVICE_NOT_FOUND";
  case DRVERR_DEVICE_ALREADY_OPEN: return "DRVERR_DEVICE_ALREADY_OPEN";
  default: return "Unknown fault.";
  }
}

#endif // XT_ENABLE_ASIO