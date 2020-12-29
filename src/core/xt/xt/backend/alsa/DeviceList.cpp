#if XT_ENABLE_ALSA
#include <xt/backend/alsa/Shared.hpp>
#include <xt/backend/alsa/Private.hpp>

#include <cstring>
#include <sstream>

XtFault
AlsaDeviceList::GetCount(int32_t* count) const
{ *count = static_cast<int32_t>(_devices.size()); return 0; }  
XtFault 
AlsaDeviceList::GetId(int32_t index, char* buffer, int32_t* size) const
{ XtiCopyString(XtiGetAlsaDeviceId(_devices[index]).c_str(), buffer, size); return 0; }

XtFault
AlsaDeviceList::GetName(char const* id, char* buffer, int32_t* size) const
{
  XtAlsaDeviceInfo info;
  if(!XtiParseAlsaDeviceInfo(id, &info)) return -ENODEV;
  std::ostringstream oss;
  oss << info.name << " (" << XtiGetAlsaNameSuffix(info.type) << ")";
  XtiCopyString(oss.str().c_str(), buffer, size);
  return 0;
}

XtFault
AlsaDeviceList::GetCapabilities(char const* id, XtDeviceCaps* capabilities) const
{
  int err;
  int flags = 0;
  snd_pcm_t* pcm;
  XtAlsaDeviceInfo info;

  if(!XtiParseAlsaDeviceInfo(id, &info)) return -ENODEV;
  bool output = XtiAlsaTypeIsOutput(info.type);
  flags |= output? XtDeviceCapsOutput: XtDeviceCapsInput;
  auto stream = output? SND_PCM_STREAM_PLAYBACK: SND_PCM_STREAM_CAPTURE;
  if((err = snd_pcm_open(&pcm, info.name.c_str(), stream, 0)) == 0)
  {
    auto pcmType = snd_pcm_type(pcm);
    if(pcmType == SND_PCM_TYPE_HW) flags |= XtDeviceCapsHwDirect;
    snd_pcm_close(pcm);
  }
  *capabilities = static_cast<XtDeviceCaps>(flags);
  return 0;
}

#endif // XT_ENABLE_ALSA