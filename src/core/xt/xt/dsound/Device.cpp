#if XT_ENABLE_DSOUND
#include <xt/dsound/Shared.hpp>
#include <xt/dsound/Private.hpp>
#include <xt/private/Win32.hpp>
#include <xt/api/private/Platform.hpp>
#include <xt/private/BlockingAdapter.hpp>
#include <memory>

XtFault 
DSoundDevice::ShowControlPanel()
{ return DS_OK; }
XtFault
DSoundDevice::GetMix(XtBool* valid, XtMix* mix) const
{ return DS_OK; }
XtFault
DSoundDevice::SupportsAccess(XtBool interleaved, XtBool* supports) const
{ *supports = interleaved; return DS_OK; }
XtFault
DSoundDevice::GetChannelName(XtBool output, int32_t index, char* buffer, int32_t* size) const
{ XtiCopyString(XtiWfxChannelNames[index], buffer, size); return DS_OK; }

XtFault
DSoundDevice::GetChannelCount(XtBool output, int32_t* count) const
{
  if(output && _output == nullptr) return S_OK;
  if(!output && _input == nullptr) return S_OK;
  *count = sizeof(XtiWfxChannelNames) / sizeof(char const*);
  return DS_OK;
}

XtFault
DSoundDevice::SupportsFormat(XtFormat const* format, XtBool* supports) const
{
  if(format->mix.rate < XtiDsMinSampleRate) return DS_OK;
  if(format->mix.rate > XtiDsMaxSampleRate) return DS_OK;
  if(_input.p == nullptr && format->channels.inputs > 0) return S_OK;
  if(_output.p == nullptr && format->channels.outputs > 0) return S_OK;
  *supports = XtTrue;
  return DS_OK;
}

XtFault
DSoundDevice::GetBufferSize(XtFormat const* format, XtBufferSize* size) const
{
  size->min = XtiDsMinBufferMs;
  size->max = XtiDsMaxBufferMs;
  size->current = XtiDsDefaultBufferMs;
  return DS_OK;
}

XtFault
DSoundDevice::OpenBlockingStream(XtDeviceStreamParams const* params, XtBlockingStream** stream)
{
  HRESULT hr;
  WAVEFORMATEXTENSIBLE wfx;
  DSBUFFERDESC renderDesc = { 0 };
  DSCBUFFERDESC captureDesc = { 0 };

  double bufferSize = params->bufferSize;
  auto const& channels = params->format.channels;
  XT_ASSERT(XtiFormatToWfx(params->format, wfx));
  auto result = std::make_unique<DSoundStream>();
  if(bufferSize < XtiDsMinBufferMs) bufferSize = XtiDsMinBufferMs;
  if(bufferSize > XtiDsMaxBufferMs) bufferSize = XtiDsMaxBufferMs;

  result->_dsProcessed = 0;
  result->_xtProcessed = 0;
  result->_previousPosition = 0;
  result->_bufferFrames = static_cast<int32_t>(bufferSize / 1000.0 * params->format.mix.rate);
  result->_frameSize = (channels.inputs + channels.outputs) * XtiGetSampleSize(params->format.mix.sample);
  DWORD bufferBytes = result->_bufferFrames * result->_frameSize;
  result->_audio = std::vector<uint8_t>(static_cast<size_t>(bufferBytes), 0);
  if(_input)
  {
    captureDesc.dwBufferBytes = bufferBytes;
    captureDesc.dwSize = sizeof(DSCBUFFERDESC);
    captureDesc.lpwfxFormat = reinterpret_cast<WAVEFORMATEX*>(&wfx);
    XT_VERIFY_COM(DirectSoundCaptureCreate8(&_id, &result->_input, nullptr));
    XT_VERIFY_COM(result->_input->CreateCaptureBuffer(&captureDesc, &result->_inputBuffer, nullptr));
  } else
  {
    renderDesc.dwBufferBytes = bufferBytes;
    renderDesc.dwSize = sizeof(DSBUFFERDESC);
    renderDesc.lpwfxFormat = reinterpret_cast<WAVEFORMATEX*>(&wfx);
    renderDesc.dwFlags = DSBCAPS_GLOBALFOCUS | DSBCAPS_TRUEPLAYPOSITION;
    auto hwnd = static_cast<HWND>(XtPlatform::instance->_window);
    XT_VERIFY_COM(DirectSoundCreate(&_id, &result->_output, nullptr));
    XT_VERIFY_COM(result->_output->SetCooperativeLevel(hwnd, DSSCL_PRIORITY));
    XT_VERIFY_COM(result->_output->CreateSoundBuffer(&renderDesc, &result->_outputBuffer, nullptr));
  }
  *stream = result.release();
  return DS_OK;
}

#endif // XT_ENABLE_DSOUND