#ifndef XT_ASIO_FAULT_HPP
#define XT_ASIO_FAULT_HPP

#include <xt/audio/Enums.h>
#include <xt/private/Shared.hpp>

#define XT_ASE_Format (static_cast<XtFault>(-993))

char const* 
XtiGetAsioFaultText(XtFault fault);
XtCause 
XtiGetAsioFaultCause(XtFault fault);

#endif // XT_ASIO_FAULT_HPP