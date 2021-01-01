#ifndef XT_API_PRINT_H
#define XT_API_PRINT_H

/** @cond */
#include <xt/api/Enums.h>
#include <xt/api/Shared.h>
#include <xt/api/Structs.h>
/** @endcond */

#ifdef __cplusplus
extern "C" {
#endif

XT_API char const* XT_CALL 
XtPrintCause(XtCause cause);
XT_API char const* XT_CALL 
XtPrintSetup(XtSetup setup);
XT_API char const* XT_CALL 
XtPrintSystem(XtSystem system);
XT_API char const* XT_CALL 
XtPrintSample(XtSample sample);
XT_API char const* XT_CALL 
XtPrintEnumFlags(XtEnumFlags flags);
XT_API char const* XT_CALL 
XtPrintErrorInfo(XtErrorInfo const* info);
XT_API char const* XT_CALL
XtPrintLocation(XtLocation const* location);
XT_API char const* XT_CALL 
XtPrintDeviceCaps(XtDeviceCaps capabilities);
XT_API char const* XT_CALL 
XtPrintServiceCaps(XtServiceCaps capabilities);

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // XT_API_PRINT_H