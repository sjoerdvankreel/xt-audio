#include <xt-audio.h>
#include <stdio.h>

static void XT_CALL OnFatal(void) {
  printf("Fatal error.\n");
}

static void XT_CALL OnTrace(XtLevel level, const char* message) {
  if (level != XtLevelInfo)
    printf("-- %s: %s\n", XtPrintLevelToString(level), message);
}

int PrintInfoMain(int argc, char** argv) {

  XtMix* mix;
  XtError err;
  int32_t s, d;
  char* text = NULL;
  int32_t count, channels;
  XtDevice* device = NULL;
  const XtService* service;

  XtAudioInit("Sample", NULL, &OnTrace, &OnFatal);
  printf("Win32: %d\n", XtAudioIsWin32());
  printf("Version: %s\n", XtAudioGetVersion());
  printf("Pro Audio: %s\n", XtServiceGetName(XtAudioGetServiceBySetup(XtSetupProAudio)));
  printf("System Audio: %s\n", XtServiceGetName(XtAudioGetServiceBySetup(XtSetupSystemAudio)));
  printf("Consumer Audio: %s\n", XtServiceGetName(XtAudioGetServiceBySetup(XtSetupConsumerAudio)));

  for (s = 0; s < XtAudioGetServiceCount(); s++) {
    service = XtAudioGetServiceByIndex(s);
    printf("Service %s:\n", XtServiceGetName(service));
    printf("  System: %s\n", XtPrintSystemToString(XtServiceGetSystem(service)));
    
    if((err = XtServiceGetDeviceCount(service, &count)) != 0)
      goto error;
    printf("  Device count: %d\n", count);
    
    text = XtPrintCapabilitiesToString(XtServiceGetCapabilities(service));
    printf("  Capabilities: %s\n", text);
    XtAudioFree(text);
    text = NULL;
    
    if((err = XtServiceOpenDefaultDevice(service, XtFalse, &device)) != 0)
      goto error;
    if(device != NULL) {
      if((err = XtDeviceGetName(device, &text)) != 0)
        goto error;
      printf("  Default input: %s\n", text);
      XtAudioFree(text);
      text = NULL;
    }
    XtDeviceDestroy(device);
    device = NULL;

    if((err = XtServiceOpenDefaultDevice(service, XtTrue, &device)) != 0)
      goto error;
    if(device != NULL) {
      if((err = XtDeviceGetName(device, &text)) != 0)
        goto error;
      printf("  Default output: %s\n", text);
      XtAudioFree(text);
      text = NULL;
    }
    XtDeviceDestroy(device);
    device = NULL;

    for(d = 0; d < count; d++) {
      if((err = XtServiceOpenDevice(service, d, &device)) != 0)
        goto error;
      
      if((err = XtDeviceGetName(device, &text)) != 0)
        goto error;
      printf("  Device %s:\n", text);
      XtAudioFree(text);
      text = NULL;
      
      printf("    System: %s\n", XtPrintSystemToString(XtDeviceGetSystem(device)));  

      if((err = XtDeviceGetMix(device, &mix)) != 0)
        goto error;
      if(mix != NULL) {
        text = XtPrintMixToString(mix);
        printf("    Current mix: %s\n", text);
        XtAudioFree(text);
        text = NULL;
      } else {
        printf("    Current mix: null\n");
      }
      XtAudioFree(mix);
      mix = NULL;

      if((err = XtDeviceGetChannelCount(device, XtFalse, &channels)) != 0)
        goto error;
      printf("    Input channels: %d\n", channels);

      if((err = XtDeviceGetChannelCount(device, XtTrue, &channels)) != 0)
        goto error;
      printf("    Output channels: %d\n", channels);

      XtDeviceDestroy(device);
      device = NULL;
    }
  }

error:
  XtAudioFree(text);
  text = NULL;
  XtDeviceDestroy(device);
  device = NULL;

  if(err != 0) {
    text = XtPrintErrorToString(err);
    printf("Error: system %s, fault %u, cause %s, text %s, message: %s.\n",
      XtPrintSystemToString(XtErrorGetSystem(err)),
      XtErrorGetFault(err),
      XtPrintCauseToString(XtErrorGetCause(err)),
      XtErrorGetText(err),
      text);
    XtAudioFree(text);
    text = NULL;
  }

  XtAudioTerminate();
  return 0;
}
