#ifdef _WIN32
#include <xt/api/public/XtAudio.h>
#include <xt/private/Win32.hpp>
#include <xt/private/Shared.hpp>

char const*
XtiWfxChannelNames[18] =
{
  XT_STRINGIFY(SPEAKER_FRONT_LEFT),
  XT_STRINGIFY(SPEAKER_FRONT_RIGHT),
  XT_STRINGIFY(SPEAKER_FRONT_CENTER),
  XT_STRINGIFY(SPEAKER_LOW_FREQUENCY),
  XT_STRINGIFY(SPEAKER_BACK_LEFT),
  XT_STRINGIFY(SPEAKER_BACK_RIGHT),
  XT_STRINGIFY(SPEAKER_FRONT_LEFT_OF_CENTER),
  XT_STRINGIFY(SPEAKER_FRONT_RIGHT_OF_CENTER),
  XT_STRINGIFY(SPEAKER_BACK_CENTER),
  XT_STRINGIFY(SPEAKER_SIDE_LEFT),
  XT_STRINGIFY(SPEAKER_SIDE_RIGHT),
  XT_STRINGIFY(SPEAKER_TOP_CENTER),
  XT_STRINGIFY(SPEAKER_TOP_FRONT_LEFT),
  XT_STRINGIFY(SPEAKER_TOP_FRONT_CENTER),
  XT_STRINGIFY(SPEAKER_TOP_FRONT_RIGHT),
  XT_STRINGIFY(SPEAKER_TOP_BACK_LEFT),
  XT_STRINGIFY(SPEAKER_TOP_BACK_CENTER),
  XT_STRINGIFY(SPEAKER_TOP_BACK_RIGHT)
};

std::string
XtiClassIdToUtf8(CLSID const& classId)
{
  LPOLESTR wide;
  XT_ASSERT(SUCCEEDED(StringFromCLSID(classId, &wide)));
  std::string result = XtiWideStringToUtf8(wide);
  CoTaskMemFree(wide);
  return result;
}

CLSID
XtiUtf8ToClassId(char const* utf8)
{
  CLSID result;
  std::wstring wide = XtiUtf8ToWideString(utf8);
  XT_ASSERT(SUCCEEDED(CLSIDFromString(wide.c_str(), &result)));
  return result;
}

std::wstring
XtiUtf8ToWideString(char const* utf8)
{
  int count;
  XT_ASSERT((count = MultiByteToWideChar(CP_UTF8, 0, utf8, -1, nullptr, 0)) > 0);
  std::wstring result(static_cast<size_t>(count) + 1, L'\0');
  XT_ASSERT(MultiByteToWideChar(CP_UTF8, 0, utf8, -1, result.data(), count) > 0);
  return result;
}

std::string
XtiWideStringToUtf8(wchar_t const* wide)
{
  int count;
  XT_ASSERT((count = WideCharToMultiByte(CP_UTF8, 0, wide, -1, nullptr, 0, nullptr, 0)) > 0);
  std::string result(static_cast<size_t>(count) + 1, '\0');
  XT_ASSERT(WideCharToMultiByte(CP_UTF8, 0, wide, -1, result.data(), count, nullptr, 0) > 0);
  return result;
}

bool
XtiWfxToFormat(WAVEFORMATEX const& wfx, XtBool output, XtFormat& format)
{  
  memset(&format, 0, sizeof(XtFormat));
  WAVEFORMATEXTENSIBLE const* wfxe = nullptr;
  if(wfx.wFormatTag == WAVE_FORMAT_EXTENSIBLE)
    wfxe = reinterpret_cast<WAVEFORMATEXTENSIBLE const*>(&wfx);  
  format.mix.rate = wfx.nSamplesPerSec;
  format.channels.inputs = output? 0: wfx.nChannels;
  format.channels.outputs = output? wfx.nChannels: 0;
  format.channels.inMask = output || wfxe == nullptr? 0: wfxe->dwChannelMask;
  format.channels.outMask = !output || wfxe == nullptr? 0: wfxe->dwChannelMask;
  
  if(wfxe == nullptr) switch(wfx.wBitsPerSample)
  {
    case 8: format.mix.sample = XtSampleUInt8; return true;
    case 16: format.mix.sample = XtSampleInt16; return true;
    case 24: format.mix.sample = XtSampleInt24; return true;
    case 32: format.mix.sample = XtSampleInt32; return true;
    default: return false;
  }
  if(wfx.wBitsPerSample != wfxe->Samples.wValidBitsPerSample)
    return false;
  if(wfxe->SubFormat == KSDATAFORMAT_SUBTYPE_IEEE_FLOAT)
    return format.mix.sample = XtSampleFloat32, true;
  return false;
}

bool
XtiFormatToWfx(XtFormat const& format, WAVEFORMATEXTENSIBLE& wfx)
{  
  memset(&wfx, 0, sizeof(WAVEFORMATEXTENSIBLE));
  if(format.channels.inputs > 0 && format.channels.outputs > 0) return false;
  auto attrs = XtAudioGetSampleAttributes(format.mix.sample);
  wfx.Format.cbSize = 22;
  wfx.Format.nSamplesPerSec = format.mix.rate;
  wfx.Format.wFormatTag = WAVE_FORMAT_EXTENSIBLE;
  wfx.Format.wBitsPerSample = XtiGetSampleSize(format.mix.sample) * 8;
  wfx.Format.nChannels = format.channels.inputs + format.channels.outputs;
  wfx.Format.nBlockAlign = wfx.Format.wBitsPerSample / 8 * wfx.Format.nChannels;
  wfx.Format.nAvgBytesPerSec = wfx.Format.nBlockAlign * format.mix.rate;
  wfx.SubFormat = attrs.isFloat? KSDATAFORMAT_SUBTYPE_IEEE_FLOAT: KSDATAFORMAT_SUBTYPE_PCM;
  wfx.Samples.wValidBitsPerSample = wfx.Format.wBitsPerSample;
  wfx.dwChannelMask = static_cast<DWORD>(format.channels.inputs? format.channels.inMask: format.channels.outMask);
  if(wfx.dwChannelMask == 0) wfx.dwChannelMask = (1U << wfx.Format.nChannels) - 1;
  return true;
}

#endif // _WIN32