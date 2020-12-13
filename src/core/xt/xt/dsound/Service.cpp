#if XT_ENABLE_DSOUND
#include <xt/dsound/Shared.hpp>
#include <xt/dsound/Private.hpp>
#include <xt/private/Win32.hpp>
#include <memory>

XtCapabilities 
DSoundService::GetCapabilities() const
{
  auto result = XtCapabilitiesAggregation | XtCapabilitiesChannelMask;
  return static_cast<XtCapabilities>(result);
}

XtFault
DSoundService::OpenDeviceList(XtEnumFlags flags, XtDeviceList** list) const
{
  HRESULT hr;
  auto result = std::make_unique<DSoundDeviceList>();
  if((flags & XtEnumFlagsInput) != 0)
    XT_VERIFY_COM(DirectSoundCaptureEnumerateW(DSoundDeviceList::EnumCallback, &result->_inputs));
  if((flags & XtEnumFlagsOutput) != 0)
    XT_VERIFY_COM(DirectSoundEnumerateW(DSoundDeviceList::EnumCallback, &result->_outputs));
  *list = result.release();
  return DS_OK;  
}

XtFault
DSoundService::GetDefaultDeviceId(XtBool output, XtBool* valid, char* buffer, int32_t* size) const
{
  GUID id;
  HRESULT hr;
  if(output) XT_VERIFY_COM(GetDeviceID(&DSDEVID_DefaultPlayback, &id));
  else XT_VERIFY_COM(GetDeviceID(&DSDEVID_DefaultCapture, &id));
  std::string result = XtiClassIdToUtf8(id);
  XtiCopyString(result.c_str(), buffer, size);
  return DS_OK;  
}

XtFault
DSoundService::OpenDevice(char const* id, XtDevice** device) const
{  
  HRESULT hr;
  XtFault fault;
  XtDeviceList* list;
  if((fault = OpenDeviceList(XtEnumFlagsAll, &list)) != 0) return fault;
  std::unique_ptr<DSoundDeviceList>

  auto result = std::make_unique<DSoundDevice>();
  DSoundD
}



static HRESULT OpenDevice(const DeviceInfo& info, XtDevice** device) {  
  HRESULT hr;
  CComPtr<IDirectSound> output;
  CComPtr<IDirectSoundCapture> input;

  if(!info.output)
    XT_VERIFY_COM(DirectSoundCaptureCreate8(&info.guid, &input, nullptr));
  else {
    XT_VERIFY_COM(DirectSoundCreate(&info.guid, &output, nullptr));
    XT_VERIFY_COM(output->SetCooperativeLevel(static_cast<HWND>(XtPlatform::instance->_window), DSSCL_PRIORITY));
  }
  *device = new DSoundDevice(info.guid, info.name, input, output);
  return S_OK;
}
#endif // XT_ENABLE_DSOUND