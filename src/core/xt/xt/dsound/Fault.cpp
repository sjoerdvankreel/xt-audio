#include <xt/dsound/Fault.hpp>
#if !XT_ENABLE_DSOUND

XtCause XtiGetDSoundFaultCause(XtFault fault) 
{ return XtCauseUnknown; }
char const* XtiGetDSoundFaultText(XtFault fault) 
{ return nullptr; }

#else // !XT_ENABLE_DSOUND 
#include <dsound.h>

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
  case DSERR_GENERIC: return "DSERR_GENERIC";
  case DSERR_NODRIVER: return "DSERR_NODRIVER";
  case DSERR_SENDLOOP: return "DSERR_SENDLOOP";
  case DSERR_BADFORMAT: return "DSERR_BADFORMAT";
  case DSERR_ALLOCATED: return "DSERR_ALLOCATED";
  case DSERR_BUFFERLOST: return "DSERR_BUFFERLOST";
  case DSERR_UNSUPPORTED: return "DSERR_UNSUPPORTED";
  case DSERR_INVALIDCALL: return "DSERR_INVALIDCALL";
  case DSERR_NOINTERFACE: return "DSERR_NOINTERFACE";
  case DSERR_OUTOFMEMORY: return "DSERR_OUTOFMEMORY";
  case DSERR_ACCESSDENIED: return "DSERR_ACCESSDENIED";
  case DSERR_INVALIDPARAM: return "DSERR_INVALIDPARAM";
  case DSERR_DS8_REQUIRED: return "DSERR_DS8_REQUIRED";
  case DSERR_NOAGGREGATION: return "DSERR_NOAGGREGATION";
  case DSERR_FXUNAVAILABLE: return "DSERR_FXUNAVAILABLE";
  case DSERR_UNINITIALIZED: return "DSERR_UNINITIALIZED";
  case DSERR_OBJECTNOTFOUND: return "DSERR_OBJECTNOTFOUND";
  case DSERR_BUFFERTOOSMALL: return "DSERR_BUFFERTOOSMALL";
  case DS_NO_VIRTUALIZATION: return "DS_NO_VIRTUALIZATION";
  case DSERR_CONTROLUNAVAIL: return "DSERR_CONTROLUNAVAIL";
  case DSERR_PRIOLEVELNEEDED: return "DSERR_PRIOLEVELNEEDED";
  case DSERR_OTHERAPPHASPRIO: return "DSERR_OTHERAPPHASPRIO";
  case DSERR_BADSENDBUFFERGUID: return "DSERR_BADSENDBUFFERGUID";
  case DSERR_ALREADYINITIALIZED: return "DSERR_ALREADYINITIALIZED";
  default: return "Unknown fault.";
  }
}

#endif // !XT_ENABLE_DSOUND