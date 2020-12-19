#ifdef __linux__
#include <xt/shared/Linux.hpp>
#include <errno.h>
#include <cstring>

XtCause
XtiGetPosixFaultCause(XtFault fault)
{
  switch(fault)
  {
  case ESRCH: return XtCauseService;
  case EINVAL: return XtCauseFormat;
  case EBUSY:
  case ENXIO:
  case EPIPE:
  case ENODEV:
  case ENOENT:
  case ESTRPIPE: return XtCauseEndpoint;
  default: return XtCauseUnknown;
  }
}

#endif // __linux__