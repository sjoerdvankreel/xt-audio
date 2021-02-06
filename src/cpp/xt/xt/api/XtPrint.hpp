#ifndef XT_API_PRINT_HPP
#define XT_API_PRINT_HPP

/** @file */
/** @cond */
#include <xt/cpp/Core.hpp>
#include <ostream>
/** @endcond */

namespace Xt {

using namespace Detail;

inline std::ostream& 
operator<<(std::ostream& os, Cause cause) 
{ return os << XtPrintCause(static_cast<XtCause>(cause)); }
inline std::ostream& 
operator<<(std::ostream& os, Setup setup) 
{ return os << XtPrintSetup(static_cast<XtSetup>(setup)); }
inline std::ostream& 
operator<<(std::ostream& os, System system) 
{ return os << XtPrintSystem(static_cast<XtSystem>(system)); }
inline std::ostream& 
operator<<(std::ostream& os, Sample sample) 
{ return os << XtPrintSample(static_cast<XtSample>(sample)); }
inline std::ostream& 
operator<<(std::ostream& os, EnumFlags flags) 
{ return os << XtPrintEnumFlags(static_cast<XtEnumFlags>(flags)); }
inline std::ostream& 
operator<<(std::ostream& os, DeviceCaps capabilities) 
{ return os << XtPrintDeviceCaps(static_cast<XtDeviceCaps>(capabilities)); }
inline std::ostream& 
operator<<(std::ostream& os, ServiceCaps capabilities) 
{ return os << XtPrintServiceCaps(static_cast<XtServiceCaps>(capabilities)); }

inline std::ostream& 
operator<<(std::ostream& os, ErrorInfo const& info) 
{
  XtErrorInfo i;
  i.fault = info.fault;
  i.service.text = info.service.text.c_str();
  i.system = static_cast<XtSystem>(info.system);
  i.service.cause = static_cast<XtCause>(info.service.cause);
  return os << XtPrintErrorInfo(&i);
}

} // namespace Xt
#endif // XT_API_PRINT_HPP