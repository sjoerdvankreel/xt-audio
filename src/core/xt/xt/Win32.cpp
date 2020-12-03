#ifdef _WIN32
#include <xt/api/public/XtAudio.h>
#include <xt/private/Shared.hpp>
#include <xt/Win32.hpp>
#include <vector>
#include <cstring>
#include <algorithm>

// ---- local ----

// ---- win32 ----

const char* XtwWfxChannelNames[18] = {
  "Front Left", "Front Right", "Front Center",
  "Low Frequency", "Back Left", "Back Right",
  "Front Left Of Center", "Front Right Of Center", "Back Center", 
  "Side Left", "Side Right", "Top Center",
  "Top Front Left", "Top Front Center", "Top Front Right",
  "Top Back Left", "Top Back Center", "Top Back Right"
};

// ---- win32 ----

std::string XtwWideStringToUtf8(const wchar_t* wide) {
  int count;
  XT_ASSERT((count = WideCharToMultiByte(CP_UTF8, 0, wide, -1, nullptr, 0, nullptr, 0)) > 0);
  std::string result(count - 1, '\0');
  XT_ASSERT(WideCharToMultiByte(CP_UTF8, 0, wide, -1, &result[0], count, nullptr, 0) > 0);
  return result;
}

bool XtwWfxToFormat(const WAVEFORMATEX& wfx, XtBool output, XtFormat& format) {  

  memset(&format, 0, sizeof(XtFormat));
  const WAVEFORMATEXTENSIBLE* wfxe = nullptr;
  if(wfx.wFormatTag == WAVE_FORMAT_EXTENSIBLE)
    wfxe = reinterpret_cast<const WAVEFORMATEXTENSIBLE*>(&wfx);
  
  format.mix.rate = wfx.nSamplesPerSec;
  format.channels.inputs = output? 0: wfx.nChannels;
  format.channels.outputs = output? wfx.nChannels: 0;
  format.channels.inMask = output || wfxe == nullptr? 0: wfxe->dwChannelMask;
  format.channels.outMask = !output || wfxe == nullptr? 0: wfxe->dwChannelMask;

  if(wfxe != nullptr && wfx.wBitsPerSample != wfxe->Samples.wValidBitsPerSample)
    return false;
  if(wfxe != nullptr && wfxe->SubFormat == KSDATAFORMAT_SUBTYPE_IEEE_FLOAT)
    format.mix.sample = XtSampleFloat32;
  else
    switch(wfx.wBitsPerSample) {
    case 8: format.mix.sample = XtSampleUInt8; break;
    case 16: format.mix.sample = XtSampleInt16; break;
    case 24: format.mix.sample = XtSampleInt24; break;
    case 32: format.mix.sample = XtSampleInt32; break;
    default: return false;
    }
  return true;
}

bool XtwFormatToWfx(const XtFormat& format, WAVEFORMATEXTENSIBLE& wfx) {  

  memset(&wfx, 0, sizeof(WAVEFORMATEXTENSIBLE));
  if(format.channels.inputs > 0 && format.channels.outputs > 0)
    return false;

  auto attributes = XtAudioGetSampleAttributes(format.mix.sample);
  wfx.Format.cbSize = 22;
  wfx.Format.nSamplesPerSec = format.mix.rate;
  wfx.Format.wFormatTag = WAVE_FORMAT_EXTENSIBLE;
  wfx.Format.nChannels = format.channels.inputs + format.channels.outputs;
  wfx.Format.wBitsPerSample = XtiGetSampleSize(format.mix.sample) * 8;
  wfx.Format.nBlockAlign = wfx.Format.wBitsPerSample / 8 * wfx.Format.nChannels;
  wfx.Format.nAvgBytesPerSec = wfx.Format.nBlockAlign * format.mix.rate;
  wfx.SubFormat = attributes.isFloat? KSDATAFORMAT_SUBTYPE_IEEE_FLOAT: KSDATAFORMAT_SUBTYPE_PCM;
  wfx.Samples.wValidBitsPerSample = wfx.Format.wBitsPerSample;
  wfx.dwChannelMask = static_cast<DWORD>(format.channels.inputs? format.channels.inMask: format.channels.outMask);
  if(wfx.dwChannelMask == 0)
    wfx.dwChannelMask = (1U << wfx.Format.nChannels) - 1;
  return true;
}

#endif // _WIN32