#ifndef XT_AUDIO_CALLBACKS_H
#define XT_AUDIO_CALLBACKS_H

#include <xt/audio/Shared.h>
#include <cstdint>

typedef void (XT_CALLBACK *XtOnXRun)(
int32_t index, void* user);
typedef void (XT_CALLBACK *XtOnError)(
struct XtLocation const* location, char const* message);
typedef void (XT_CALLBACK *XtOnBuffer)(
XtStream const* stream, struct XtBuffer const* buffer, void* user);

#endif // XT_AUDIO_CALLBACKS_H