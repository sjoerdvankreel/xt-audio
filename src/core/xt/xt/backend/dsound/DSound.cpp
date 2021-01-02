#if XT_ENABLE_DSOUND
#include <xt/backend/dsound/Shared.hpp>
#include <xt/backend/dsound/Private.hpp>

#include <dsound.h>
#include <memory>

std::unique_ptr<XtService>
XtiCreateDSoundService()
{ return std::make_unique<DSoundService>(); }

XtServiceError
XtiGetDSoundError(XtFault fault)
{
  XtServiceError result;
  result.text = XtiGetDSoundFaultText(fault);
  result.cause = XtiGetDSoundFaultCause(fault);
  return result;
}

XtCause 
XtiGetDSoundFaultCause(XtFault fault)
{
  switch(fault) 
  {
  case DSERR_BADFORMAT: return XtCauseFormat;
  case DSERR_NODRIVER: 
  case DSERR_ALLOCATED: 
  case DSERR_BUFFERLOST: 
  case DSERR_ACCESSDENIED: 
  case DSERR_OTHERAPPHASPRIO: return XtCauseEndpoint;
  default: return XtCauseUnknown;
  }
}

char const* 
XtiGetDSoundFaultText(XtFault fault)
{
  switch(fault) 
  {
    case DS_OK: return XT_STRINGIFY(DS_OK);
    case DS_NO_VIRTUALIZATION: return XT_STRINGIFY(DS_NO_VIRTUALIZATION);
    case DSERR_ALLOCATED: return XT_STRINGIFY(DSERR_ALLOCATED);
    case DSERR_GENERIC: return XT_STRINGIFY(DSERR_GENERIC);
    case DSERR_NODRIVER: return XT_STRINGIFY(DSERR_NODRIVER);
    case DSERR_SENDLOOP: return XT_STRINGIFY(DSERR_SENDLOOP);
    case DSERR_BADFORMAT: return XT_STRINGIFY(DSERR_BADFORMAT);
    case DSERR_BUFFERLOST: return XT_STRINGIFY(DSERR_BUFFERLOST);
    case DSERR_UNSUPPORTED: return XT_STRINGIFY(DSERR_UNSUPPORTED);
    case DSERR_OUTOFMEMORY: return XT_STRINGIFY(DSERR_OUTOFMEMORY);
    case DSERR_NOINTERFACE: return XT_STRINGIFY(DSERR_NOINTERFACE);
    case DSERR_INVALIDCALL: return XT_STRINGIFY(DSERR_INVALIDCALL);
    case DSERR_INVALIDPARAM: return XT_STRINGIFY(DSERR_INVALIDPARAM);
    case DSERR_ACCESSDENIED: return XT_STRINGIFY(DSERR_ACCESSDENIED);
    case DSERR_DS8_REQUIRED: return XT_STRINGIFY(DSERR_DS8_REQUIRED);
    case DSERR_UNINITIALIZED: return XT_STRINGIFY(DSERR_UNINITIALIZED);
    case DSERR_NOAGGREGATION: return XT_STRINGIFY(DSERR_NOAGGREGATION);
    case DSERR_FXUNAVAILABLE: return XT_STRINGIFY(DSERR_FXUNAVAILABLE);
    case DSERR_CONTROLUNAVAIL: return XT_STRINGIFY(DSERR_CONTROLUNAVAIL);
    case DSERR_BUFFERTOOSMALL: return XT_STRINGIFY(DSERR_BUFFERTOOSMALL);
    case DSERR_OBJECTNOTFOUND: return XT_STRINGIFY(DSERR_OBJECTNOTFOUND);
    case DSERR_OTHERAPPHASPRIO: return XT_STRINGIFY(DSERR_OTHERAPPHASPRIO);
    case DSERR_PRIOLEVELNEEDED: return XT_STRINGIFY(DSERR_PRIOLEVELNEEDED);
    case DSERR_BADSENDBUFFERGUID: return XT_STRINGIFY(DSERR_BADSENDBUFFERGUID);
    case DSERR_ALREADYINITIALIZED: return XT_STRINGIFY(DSERR_ALREADYINITIALIZED);
    default: return "Unknown fault.";
  }
}

#endif // XT_ENABLE_DSOUND