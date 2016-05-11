#include <xt-audio.h>
#include <stdio.h>

int PrintDevicesMain(int argc, char** argv) {
  
  XtError err;
  int32_t s, d;
  int32_t count;
  char* name = NULL;
  XtDevice* device = NULL;
  const XtService* service;

  XtAudioInit(NULL, NULL, NULL, NULL);
  for (s = 0; s < XtAudioGetServiceCount(); s++) {
    service = XtAudioGetServiceByIndex(s);
    if((err = XtServiceGetDeviceCount(service, &count)) != 0)
      goto error;
    for(d = 0; d < count; d++) {
      if((err = XtServiceOpenDevice(service, d, &device)) != 0)
        goto error;
      if((err = XtDeviceGetName(device, &name)) != 0)
        goto error;
      printf("%s: %s\n", XtServiceGetName(service), name);
      XtAudioFree(name);
      name = NULL;
      XtDeviceDestroy(device);
      device = NULL;
    }
  }

error:
  XtAudioFree(name);
  XtDeviceDestroy(device);
  XtAudioTerminate();
  return 0;
}
