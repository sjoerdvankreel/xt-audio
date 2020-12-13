#if XT_ENABLE_DSOUND
#include <xt/dsound/Shared.hpp>
#include <xt/dsound/Private.hpp>
#include <xt/private/Win32.hpp>
#include <xt/api/private/Platform.hpp>
#include <memory>

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
  CComPtr<IDirectSound> output;
  CComPtr<IDirectSoundCapture> input;

  if((fault = OpenDeviceList(XtEnumFlagsAll, &list)) != DS_OK) return fault;
  std::unique_ptr<DSoundDeviceList> dsList(dynamic_cast<DSoundDeviceList*>(list));
  if((fault = dsList->GetDeviceInfo(id, &info)) != DS_OK) return fault;
  GUID guid = XtiUtf8ToClassId(info.id.c_str());

  if(info.output)
  { 
    auto hwnd = static_cast<HWND>(XtPlatform::instance->_window);
    XT_VERIFY_COM(DirectSoundCreate(&guid, &output, nullptr));
    XT_VERIFY_COM(output->SetCooperativeLevel(hwnd, DSSCL_PRIORITY));
  }
  else
    XT_VERIFY_COM(DirectSoundCaptureCreate8(&guid, &input, nullptr));
  auto result = std::make_unique<DSoundDevice>();
  result->_input = input;
  result->_output = output;
  *device = result.release();
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