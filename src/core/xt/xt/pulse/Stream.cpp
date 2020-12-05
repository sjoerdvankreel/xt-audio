#if XT_ENABLE_PULSE
#include <xt/pulse/Shared.hpp>
#include <pulse/pulseaudio.h>
#include <utility>

PulseStream::
PulseStream(bool secondary, XtPaSimple&& c, bool output, int32_t bufferFrames, int32_t frameSize):
XtBlockingStream(secondary),
_output(output), 
_client(std::move(c)), 
_audio(static_cast<size_t>(bufferFrames * frameSize), 0),
_bufferFrames(bufferFrames) 
{ XT_ASSERT(_client.pa != nullptr); }

#endif // XT_ENABLE_PULSE