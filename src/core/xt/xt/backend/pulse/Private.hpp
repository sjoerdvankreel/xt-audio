#ifndef XT_PULSE_PRIVATE_HPP
#define XT_PULSE_PRIVATE_HPP
#if XT_ENABLE_PULSE
#include <pulse/simple.h>
#include <pulse/pulseaudio.h>

inline int32_t const
XtiPaMinRate = 1;
inline int32_t const
XtiPaMaxRate = 192000;
inline int32_t const
XtiPaDefaultRate = 44100;
inline int32_t const
XtiPaDefaultChannels = 2;

inline double const
XtiPaMinBufferSize = 1.0;
inline double const
XtiPaMaxBufferSize = 2000.0;
inline double const
XtiPaDefaultBufferSize = 80.0;

inline XtFault const
XT_PA_ERR_FORMAT = PA_ERR_MAX + 1;
inline XtSample const
XtiPaDefaultSample = XtSampleInt16;

pa_sample_format
XtiSampleToPulse(XtSample sample);
XtCause
XtiGetPulseFaultCause(XtFault fault);
int
XtiCreatePulseDefaultClient(XtBool output, pa_simple** pa);

struct XtPaSimple
{
  pa_simple* pa;
  XtPaSimple(XtPaSimple const&) = delete;
  XtPaSimple& operator=(XtPaSimple const&) = delete;

  XtPaSimple(): pa(nullptr) {}
  XtPaSimple(pa_simple* pa): pa(pa) { }
  ~XtPaSimple() { if(pa != nullptr) pa_simple_free(pa); }
  XtPaSimple(XtPaSimple&& rhs): pa(rhs.pa) { rhs.pa = nullptr; }
  XtPaSimple& operator=(XtPaSimple&& rhs) { pa = rhs.pa; rhs.pa = nullptr; return *this; }
};

#endif // XT_ENABLE_PULSE
#endif // XT_PULSE_PRIVATE_HPP