#include <xt-audio.h>
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif
#include <stdio.h>
#include <limits.h>
#include <stdlib.h>

static void XT_CALL RenderCallback(
  const XtStream* stream, const void* input, void* output, int32_t frames, 
  double time, uint64_t position, XtBool timeValid, uint64_t error, void* user) {

  int32_t f, c;
  double noise;
  const XtFormat* format;

  format = XtStreamGetFormat(stream);
  short* buffer = (short*)(output);
  for(f = 0; f < frames; f++)
    for(c = 0; c < format->outputs; c++) {
      noise = rand() / (double)(RAND_MAX) * 2.0 - 1.0;
      buffer[f * format->outputs + c] = (short)(noise * SHRT_MAX);
    }
}

int RenderMain(int argc, char** argv) {

  XtError err;
  XtBool supports;
  XtBuffer buffer;
  XtDevice* device = NULL;
  XtStream* stream = NULL;
  XtFormat format = { 0 };
  const XtService* service;

  XtAudioInit(NULL, NULL, NULL, NULL);
  service = XtAudioGetServiceBySetup(XtSetupConsumerAudio);
  if((err = XtServiceOpenDefaultDevice(service, XtTrue, &device)) != 0)
    goto error;

  if(device == NULL) {
    printf("No default device found.\n");
    goto error;
  }

  format.outputs = 2;
  format.mix.rate = 44100;
  format.mix.sample = XtSampleInt16;
  if((err = XtDeviceSupportsFormat(device, &format, &supports)) != 0)
    goto error;
  if(!supports) {
    printf("Format not supported.\n");
    goto error;
  }

  if((err = XtDeviceGetBuffer(device, &format, &buffer)) != 0)
    goto error;
  if((err = XtDeviceOpenStream(device, &format, XtTrue,
    buffer.current, &RenderCallback, NULL, &stream)) != 0)
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
  XtStreamDestroy(stream);
  XtDeviceDestroy(device);
  XtAudioTerminate();
  return 0;
}
