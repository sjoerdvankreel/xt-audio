#ifndef XT_AUDIO_PRINT_H
#define XT_AUDIO_PRINT_H

#include <xt/audio/Enums.h>
#include <xt/audio/Shared.h>
#include <xt/audio/Structs.h>

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
#endif // XT_AUDIO_PRINT_H