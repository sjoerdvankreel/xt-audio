#ifndef XT_API_PRINT_H
#define XT_API_PRINT_H

#include <xt/api/Enums.h>
#include <xt/api/Shared.h>
#include <xt/api/Structs.h>

#ifdef __cplusplus
extern "C" {
#endif

XT_API char const* XT_CALL 
XtPrintCauseToString(XtCause cause);
XT_API char const* XT_CALL 
XtPrintSetupToString(XtSetup setup);
XT_API char const* XT_CALL 
XtPrintSystemToString(XtSystem system);
XT_API char const* XT_CALL 
XtPrintSampleToString(XtSample sample);
XT_API char const* XT_CALL 
XtPrintErrorInfoToString(XtErrorInfo const* info);
XT_API char const* XT_CALL
XtPrintLocationToString(XtLocation const* location);
XT_API char const* XT_CALL 
XtPrintCapabilitiesToString(XtCapabilities capabilities);

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // XT_API_PRINT_H