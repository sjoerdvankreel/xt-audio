#ifndef XT_JACK_PRIVATE_HPP
#define XT_JACK_PRIVATE_HPP
#if XT_ENABLE_JACK
#include <jack/jack.h>
#include <vector>
#include <cstdint>

template <class T>
struct XtJackPtr
{
  T* const p;
  ~XtJackPtr() { jack_free(p); }
  XtJackPtr(T* p): p(p) { XT_ASSERT(p != nullptr); }
};

struct XtJackClient
{
  jack_client_t* jc;
  XtJackClient(XtJackClient const&) = delete;
  XtJackClient& operator=(XtJackClient const&) = delete;
  ~XtJackClient() { if(jc != nullptr) jack_client_close(jc); }
  
  XtJackClient(jack_client_t* jc): jc(jc) { }
  XtJackClient(XtJackClient&& rhs): jc(rhs.jc) { rhs.jc = nullptr; }
  XtJackClient& operator=(XtJackClient&& rhs) { jc = rhs.jc; rhs.jc = nullptr; return *this; }
};

struct XtJackConnection
{
  char const* dest;
  char const* source;
  jack_client_t* jc;

  XtJackConnection& operator=(XtJackConnection&& rhs)
  { dest = rhs.dest; source = rhs.source; jc = rhs.jc; rhs.source = nullptr; return *this; }
  XtJackConnection(jack_client_t* jc, char const* source, char const* dest): dest(dest), source(source), jc(jc)
  { XT_ASSERT(jc != nullptr); XT_ASSERT(source != nullptr); XT_ASSERT(dest != nullptr); }

  XtJackConnection(XtJackConnection const&) = delete;
  XtJackConnection& operator=(XtJackConnection const&) = delete;
  ~XtJackConnection() { if(source != nullptr) jack_disconnect(jc, source, dest); }
  XtJackConnection(XtJackConnection&& rhs): dest(rhs.dest), source(rhs.source), jc(rhs.jc) { rhs.source = nullptr; }
};

struct XtJackPort
{
  jack_port_t* port;
  jack_client_t* jc;
  char const* connectTo;

  XtJackPort(XtJackPort const&) = delete;
  XtJackPort& operator=(XtJackPort const&) = delete;
  ~XtJackPort() { if(port != nullptr) jack_port_unregister(jc, port); }

  XtJackPort(XtJackPort&& rhs): port(rhs.port), jc(rhs.jc), connectTo(rhs.connectTo) { rhs.port = nullptr; }
  XtJackPort& operator=(XtJackPort&& rhs) { jc = rhs.jc; port = rhs.port; connectTo = rhs.connectTo; rhs.port = nullptr; return *this; }
  XtJackPort(jack_client_t* jc, jack_port_t* port): port(port), jc(jc), connectTo(nullptr) { XT_ASSERT(jc != nullptr); XT_ASSERT(port != nullptr); }
};

void
XtiJackSilentCallback(char const*);
void
XtiJackErrorCallback(char const* msg);
XtFault
XtiJackCreatePorts(jack_client_t* jc, uint32_t channels, uint64_t mask, unsigned long flag, std::vector<XtJackPort>& result);

#endif // XT_ENABLE_JACK
#endif // XT_JACK_PRIVATE_HPP