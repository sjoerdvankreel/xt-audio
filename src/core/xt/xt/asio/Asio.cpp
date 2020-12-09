#if XT_ENABLE_ASIO
#include <xt/asio/Shared.hpp>
#include <xt/asio/Private.hpp>
#include <memory>

bool
XtiIsAsioSuccess(ASIOError e)
{ return e == ASE_OK || e == ASE_SUCCESS; }

std::unique_ptr<XtService>
XtiCreateAsioService()
{ return std::make_unique<AsioService>(); }

XtServiceError
XtiGetAsioError(XtFault fault)
{
  XtServiceError result;
  result.text = XtiGetAsioFaultText(fault);
  result.cause = XtiGetAsioFaultCause(fault);
  return result;
}

XtCause
XtiGetAsioFaultCause(XtFault fault)
{
  switch(fault) 
  {
  case ASE_NoClock:
  case ASE_NotPresent:
  case DRVERR_DEVICE_NOT_FOUND:
  case DRVERR_DEVICE_ALREADY_OPEN: return XtCauseEndpoint;
  case XT_ASE_Format: return XtCauseFormat;
  default: return XtCauseUnknown;
  }
}

bool
XtiSampleToAsio(XtSample sample, ASIOSampleType& asio)
{
  switch(sample)
  {
  case XtSampleInt16: asio = ASIOSTInt16LSB; return true;
  case XtSampleInt24: asio = ASIOSTInt24LSB; return true;
  case XtSampleInt32: asio = ASIOSTInt32LSB; return true;
  case XtSampleFloat32: asio = ASIOSTFloat32LSB; return true;
  default: return false;
  }
}

bool
XtiSampleFromAsio(ASIOSampleType asio, XtSample& sample)
{
  switch(asio)
  {
  case ASIOSTInt16LSB: sample = XtSampleInt16; return true;
  case ASIOSTInt24LSB: sample = XtSampleInt24; return true;
  case ASIOSTInt32LSB: sample = XtSampleInt32; return true;
  case ASIOSTFloat32LSB: sample = XtSampleFloat32; return true;
  default: return false;
  }
}

bool 
XtiIsAsioChannelInUse(int32_t count, uint64_t mask, long channel)
{
  if(mask == 0 && channel < count) return true;
  return mask != 0 && ((mask >> channel) & 1ULL) == 1ULL;
}

ASIOError
XtiGetAsioChannelInfos(IASIO* asio, std::vector<ASIOChannelInfo>& infos)
{
  long inputs, outputs;
  XT_VERIFY_ASIO(asio->getChannels(&inputs, &outputs));
  XT_VERIFY_ASIO(XtiGetAsioChannelInfos(asio, XtFalse, inputs, infos));
  XT_VERIFY_ASIO(XtiGetAsioChannelInfos(asio, XtTrue, outputs, infos));
  return ASE_OK;
}

ASIOError
XtiGetAsioChannelInfo(IASIO* asio, XtBool output, int32_t index, ASIOChannelInfo& info)
{
  info.channel = index;
  info.isInput = !output;
  XT_VERIFY_ASIO(asio->getChannelInfo(&info));
  return ASE_OK;
}

ASIOError
XtiGetAsioChannelInfos(IASIO* asio, XtBool output, long channels, std::vector<ASIOChannelInfo>& infos)
{
  for(long i = 0; i < channels; i++)
  {
    ASIOChannelInfo info = { 0 };
    XT_VERIFY_ASIO(XtiGetAsioChannelInfo(asio, output, i, info));
    infos.push_back(info);
  }
  return ASE_OK;
}

char const* 
XtiGetAsioFaultText(XtFault fault)
{
  switch(fault) 
  {
  case ASE_OK: return XT_STRINGIFY(ASE_OK);
  case ASE_SUCCESS: return XT_STRINGIFY(ASE_SUCCESS);
  case ASE_NoClock: return XT_STRINGIFY(ASE_NoClock);
  case ASE_NoMemory: return XT_STRINGIFY(ASE_NoMemory);
  case ASE_NotPresent: return XT_STRINGIFY(ASE_NotPresent);
  case ASE_InvalidMode: return XT_STRINGIFY(ASE_InvalidMode);
  case ASE_HWMalfunction: return XT_STRINGIFY(ASE_HWMalfunction);
  case ASE_SPNotAdvancing: return XT_STRINGIFY(ASE_SPNotAdvancing);
  case ASE_InvalidParameter: return XT_STRINGIFY(ASE_InvalidParameter);
  case XT_ASE_Format: return XT_STRINGIFY(XT_ASE_Format);
  case DRVERR_INVALID_PARAM: return XT_STRINGIFY(DRVERR_INVALID_PARAM);
  case DRVERR_DEVICE_NOT_FOUND: return XT_STRINGIFY(DRVERR_DEVICE_NOT_FOUND);
  case DRVERR_DEVICE_ALREADY_OPEN: return XT_STRINGIFY(DRVERR_DEVICE_ALREADY_OPEN);
  default: return "Unknown fault.";
  }
}

#endif // XT_ENABLE_ASIO