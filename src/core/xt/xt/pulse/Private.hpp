#ifndef XT_PULSE_PRIVATE_HPP
#define XT_PULSE_PRIVATE_HPP
#if XT_ENABLE_PULSE
#include <pulse/simple.h>
#include <pulse/pulseaudio.h>

struct XtPaSimple
{
  pa_simple* pa;
  XtPaSimple(XtPaSimple const&) = delete;
  XtPaSimple& operator=(XtPaSimple const&) = delete;

  XtPaSimple(pa_simple* pa): pa(pa) { }
  ~XtPaSimple() { if(pa != nullptr) pa_simple_free(pa); }
  XtPaSimple(XtPaSimple&& rhs): pa(rhs.pa) { rhs.pa = nullptr; }
  XtPaSimple& operator=(XtPaSimple&& rhs) { pa = rhs.pa; rhs.pa = nullptr; return *this; }
};

#endif // XT_ENABLE_PULSE
#endif // XT_PULSE_PRIVATE_HPP