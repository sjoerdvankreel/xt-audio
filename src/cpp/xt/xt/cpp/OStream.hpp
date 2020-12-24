#ifndef XT_CPP_OSTREAM_HPP
#define XT_CPP_OSTREAM_HPP

/** @file */

#include <xt/cpp/Core.hpp>

#include <ostream>

namespace Xt {

using namespace Detail;

inline std::ostream& 
operator<<(std::ostream& os, Cause cause) 
{ return os << XtPrintCauseToString(static_cast<XtCause>(cause)); }
inline std::ostream& 
operator<<(std::ostream& os, Setup setup) 
{ return os << XtPrintSetupToString(static_cast<XtSetup>(setup)); }
inline std::ostream& 
operator<<(std::ostream& os, System system) 
{ return os << XtPrintSystemToString(static_cast<XtSystem>(system)); }
inline std::ostream& 
operator<<(std::ostream& os, Sample sample) 
{ return os << XtPrintSampleToString(static_cast<XtSample>(sample)); }
inline std::ostream& 
operator<<(std::ostream& os, ServiceCaps capabilities) 
{ return os << XtPrintServiceCapsToString(static_cast<XtServiceCaps>(capabilities)); }

inline std::ostream& 
operator<<(std::ostream& os, Location const& location) 
{
  XtLocation loc;
  loc.line = location.line;
  loc.file = location.file.c_str();
  loc.func = location.func.c_str();
  return os << XtPrintLocationToString(&loc);
}

inline std::ostream& 
operator<<(std::ostream& os, ErrorInfo const& info) 
{
  XtErrorInfo i;
  i.fault = info.fault;
  i.service.text = info.service.text.c_str();
  i.system = static_cast<XtSystem>(info.system);
  i.service.cause = static_cast<XtCause>(info.service.cause);
  return os << XtPrintErrorInfoToString(&i);
}

} // namespace Xt
#endif // XT_CPP_OSTREAM_HPP