#if XT_ENABLE_DSOUND
#include <xt/shared/Win32.hpp>
#include <xt/private/Platform.hpp>
#include <xt/backend/dsound/Shared.hpp>
#include <xt/backend/dsound/Private.hpp>
#include <memory>

XtFault
DSoundService::GetFormatFault() const
{ return DSERR_BADFORMAT; }

XtCapabilities 
DSoundService::GetCapabilities() const
{
  auto result = XtCapabilitiesAggregation | XtCapabilitiesChannelMask;
  return static_cast<XtCapabilities>(result);
}

XtFault
DSoundService::OpenDevice(char const* id, XtDevice** device) const
{  
  HRESULT hr;
  XtFault fault;
  XtDeviceList* list;
  XtDsDeviceInfo info;  
  if((fault = OpenDeviceList(XtEnumFlagsAll, &list)) != DS_OK) return fault;
  std::unique_ptr<DSoundDeviceList> dsList(dynamic_cast<DSoundDeviceList*>(list));
  if((fault = dsList->GetDeviceInfo(id, &info)) != DS_OK) return fault;

  auto result = std::make_unique<DSoundDevice>();
  result->_id = info.id;
  if(info.output)
  { 
    auto hwnd = static_cast<HWND>(XtPlatform::instance->_window);
    XT_VERIFY_COM(DirectSoundCreate(&info.id, &result->_output, nullptr));
    XT_VERIFY_COM(result->_output->SetCooperativeLevel(hwnd, DSSCL_PRIORITY));
  }
  else
    XT_VERIFY_COM(DirectSoundCaptureCreate8(&info.id, &result->_input, nullptr));
  *device = result.release();
  return DS_OK; 
}

XtFault
DSoundService::GetDefaultDeviceId(XtBool output, XtBool* valid, char* buffer, int32_t* size) const
{
  GUID id;
  HRESULT hr;
  if(output) hr = GetDeviceID(&DSDEVID_DefaultPlayback, &id);
  else hr = GetDeviceID(&DSDEVID_DefaultCapture, &id);
  if(hr == DSERR_NODRIVER) return DS_OK;
  if(FAILED(hr)) return hr;
  *valid = XtTrue;
  std::string result = XtiClassIdToUtf8(id);
  XtiCopyString(result.c_str(), buffer, size);
  return DS_OK;  
}

XtFault
DSoundService::OpenDeviceList(XtEnumFlags flags, XtDeviceList** list) const
{
  HRESULT hr;
  size_t inputs = 0;
  auto result = std::make_unique<DSoundDeviceList>();
  if((flags & XtEnumFlagsInput) != 0)
  {
    XT_VERIFY_COM(DirectSoundCaptureEnumerateW(DSoundDeviceList::EnumCallback, &result->_devices));
    inputs = result->_devices.size();
  }
  if((flags & XtEnumFlagsOutput) != 0)
    XT_VERIFY_COM(DirectSoundEnumerateW(DSoundDeviceList::EnumCallback, &result->_devices));
  for(size_t i = 0; i < inputs; i++)
    result->_devices[i].output = false;
  for(size_t i = inputs; i < result->_devices.size(); i++)
    result->_devices[i].output = true;
  *list = result.release();
  return DS_OK;  
}

#endif // XT_ENABLE_DSOUND