#ifndef XT_ASIO_PRIVATE_HPP
#define XT_ASIO_PRIVATE_HPP
#if XT_ENABLE_ASIO

#include <xt/asio/Shared.hpp>
#include <xt/private/Win32.hpp>
#include <xt/private/Shared.hpp>
#include <asmjit/asmjit.h>
#include <vector>

#define XT_VERIFY_ASIO(c)  \
  do { auto e = (c);       \
  if(!XtiIsAsioSuccess(e)) \
  return XT_TRACE(#c), e; } while(0)

#define XT_ASIO_CALL __cdecl
#define XT_ASE_Format (static_cast<XtFault>(-1001))

typedef void (XT_ASIO_CALL*
XtAsioSdkBufferSwitch)(long, ASIOBool);
typedef void (XT_ASIO_CALL*
XtAsioContextBufferSwitch)(void*, long, ASIOBool);
typedef ASIOTime* (XT_ASIO_CALL*
XtAsioSdkBufferSwitchTimeInfo)(ASIOTime*, long, ASIOBool);
typedef ASIOTime* (XT_ASIO_CALL*
XtAsioContextBufferSwitchTimeInfo)(void*, ASIOTime*, long, ASIOBool);

bool
XtiIsAsioSuccess(ASIOError e);
char const* 
XtiGetAsioFaultText(XtFault fault);
XtCause 
XtiGetAsioFaultCause(XtFault fault);
bool
XtiSampleToAsio(XtSample sample, ASIOSampleType& asio);
bool
XtiSampleFromAsio(ASIOSampleType asio, XtSample& sample);
bool 
XtiIsAsioChannelInUse(int32_t count, uint64_t mask, long channel);
ASIOError
XtiGetAsioChannelInfos(IASIO* asio, std::vector<ASIOChannelInfo>& infos);
ASIOError
XtiGetAsioChannelInfo(IASIO* asio, XtBool output, int32_t index, ASIOChannelInfo& info);
ASIOError
XtiGetAsioChannelInfos(IASIO* asio, XtBool output, long channels, std::vector<ASIOChannelInfo>& infos);

XtAsioSdkBufferSwitch
XtiAsioJitBufferSwitch(asmjit::JitRuntime* runtime, XtAsioContextBufferSwitch target, void* ctx);
XtAsioSdkBufferSwitchTimeInfo
XtiAsioJitBufferSwitchTimeInfo(asmjit::JitRuntime* runtime, XtAsioContextBufferSwitchTimeInfo target, void* ctx);

#endif // XT_ENABLE_ASIO
#endif // XT_ASIO_PRIVATE_HPP