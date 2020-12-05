#ifndef XT_PULSE_PRIVATE_HPP
#define XT_PULSE_PRIVATE_HPP
#if XT_ENABLE_PULSE
#include <pulse/simple.h>
#include <pulse/pulseaudio.h>

struct XtPaSimple
{
  pa_simple* _simple;
  XtPaSimple(XtPaSimple const&) = delete;
  XtPaSimple& operator=(XtPaSimple const&) = delete;

  XtPaSimple(pa_simple* simple): _simple(simple) { }
  ~XtPaSimple() { if(_simple != nullptr) pa_simple_free(_simple); }
  XtPaSimple(XtPaSimple&& rhs): _simple(rhs._simple) { rhs._simple = nullptr; }
  XtPaSimple& operator=(XtPaSimple&& rhs) { _simple = rhs._simple; rhs._simple = nullptr; return *this; }
};

#endif // XT_ENABLE_PULSE
#endif // XT_PULSE_PRIVATE_HPP