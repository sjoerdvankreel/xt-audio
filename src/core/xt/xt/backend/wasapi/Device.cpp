#if XT_ENABLE_WASAPI
#include <xt/shared/Win32.hpp>
#include <xt/backend/wasapi/Shared.hpp>
#include <xt/backend/wasapi/Private.hpp>

XtFault 
WasapiDevice::ShowControlPanel()
{ return S_OK; }
void*
WasapiDevice::GetHandle() const
{ return _client.p; }
XtFault
WasapiDevice::SupportsAccess(XtBool interleaved, XtBool* supports) const
{ *supports = interleaved; return S_OK; }
XtFault
WasapiDevice::GetChannelName(XtBool output, int32_t index, char* buffer, int32_t* size) const
{ XtiCopyString(XtiWfxChannelNames[index], buffer, size); return S_OK; }

XtFault
WasapiDevice::GetChannelCount(XtBool output, int32_t* count) const
{ 
  bool isOutput = XtiWasapiTypeIsOutput(_type);
  return (output != XtFalse) == isOutput? sizeof(XtiWfxChannelNames) / sizeof(const char*): 0;
  return S_OK;
}

#endif // XT_ENABLE_WASAPI