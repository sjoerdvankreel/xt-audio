#ifndef XT_CPP_OSTREAM_HPP
#define XT_CPP_OSTREAM_HPP

/** @file */

#include <xt/cpp/Core.hpp>
#include <xt/cpp/XtDevice.hpp>

#include <ostream>

namespace Xt {

using namespace Detail;

inline std::ostream& operator<<(std::ostream& os, Device const& device) 
{ return os << device.GetName(); }
inline std::ostream& operator<<(std::ostream& os, Cause cause) 
{ return os << XtPrintCauseToString(static_cast<XtCause>(cause)); }
inline std::ostream& operator<<(std::ostream& os, Setup setup) 
{ return os << XtPrintSetupToString(static_cast<XtSetup>(setup)); }
inline std::ostream& operator<<(std::ostream& os, System system) 
{ return os << XtPrintSystemToString(static_cast<XtSystem>(system)); }
inline std::ostream& operator<<(std::ostream& os, Sample sample) 
{ return os << XtPrintSampleToString(static_cast<XtSample>(sample)); }
inline std::ostream& operator<<(std::ostream& os, Capabilities capabilities) 
{ return os << XtPrintCapabilitiesToString(static_cast<XtCapabilities>(capabilities)); }

inline std::ostream& 
operator<<(std::ostream& os, ErrorInfo const& info) 
{
  XtErrorInfo i;
  i.fault = info.fault;
  i.text = info.text.c_str();
  i.cause = static_cast<XtCause>(info.cause);
  i.system = static_cast<XtSystem>(info.system);
  return os << XtPrintErrorInfoToString(&i);
}

} // namespace Xt
#endif // XT_CPP_OSTREAM_HPP