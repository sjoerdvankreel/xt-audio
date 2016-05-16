#include <xt-audio.h>
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif
#include <stdio.h>
#include <stdlib.h>

static void XT_CALL CaptureCallback(
  const XtStream* stream, const void* input, void* output, int32_t frames, 
  double time, uint64_t position, XtBool timeValid, uint64_t error, void* user) {

  int32_t bufferSize;
  const XtFormat* format;
  XtAttributes attributes;
  
  format = XtStreamGetFormat(stream);
  XtAudioGetSampleAttributes(format->mix.sample, &attributes);
  bufferSize = frames * format->inputs * attributes.size;

  if(frames > 0)
    // Don't do this.
    fwrite(input, 1, bufferSize, (FILE*)(user));
}

int CaptureMain(int argc, char** argv) {

  XtError err;
  XtBool supports;
  XtBuffer buffer;
  FILE* recording = NULL;
  XtDevice* device = NULL;
  XtStream* stream = NULL;
  XtFormat format = { 0 };
  const XtService* service;

  XtAudioInit(NULL, NULL, NULL, NULL);
  service = XtAudioGetServiceBySetup(XtSetupConsumerAudio);
  if((err = XtServiceOpenDefaultDevice(service, XtFalse, &device)) != 0)
    goto error;

  if(device == NULL) {
    printf("No default device found.\n");
    goto error;
  }

  format.inputs = 2;
  format.mix.rate = 44100;
  format.mix.sample = XtSampleInt24;
  if((err = XtDeviceSupportsFormat(device, &format, &supports)) != 0)
    goto error;
  if(!supports) {
    printf("Format not supported.\n");
    goto error;
  }

  if((err = XtDeviceGetBuffer(device, &format, &buffer)) != 0)
    goto error;

  recording = fopen("xt-audio.raw", "wb");
  if((err = XtDeviceOpenStream(device, &format, XtTrue, 
    buffer.current, &CaptureCallback, recording, &stream)) != 0)
    goto error;

  if((err = XtStreamStart(stream)) != 0)
    goto error;
#if _WIN32
    Sleep(1000);
#else
    usleep(1000 * 1000);
#endif
  if((err = XtStreamStop(stream)) != 0)
    goto error;

error:
  if(recording != NULL)
    fclose(recording);
  XtStreamDestroy(stream);
  XtDeviceDestroy(device);
  XtAudioTerminate();
  return 0;
}
