#ifndef XT_AUDIO_ENUMS_H
#define XT_AUDIO_ENUMS_H

typedef enum XtSetup { XtSetupProAudio, XtSetupSystemAudio, XtSetupConsumerAudio } XtSetup;
typedef enum XtSample { XtSampleUInt8, XtSampleInt16, XtSampleInt24, XtSampleInt32, XtSampleFloat32 } XtSample;
typedef enum XtCause { XtCauseFormat, XtCauseService, XtCauseGeneric, XtCauseUnknown, XtCauseEndpoint } XtCause;
typedef enum XtSystem { XtSystemALSA = 1, XtSystemASIO, XtSystemJACK, XtSystemWASAPI, XtSystemPulseAudio, XtSystemDSound } XtSystem;
typedef enum XtCapabilities { XtCapabilitiesNone = 0x0, XtCapabilitiesTime = 0x1, XtCapabilitiesLatency = 0x2, XtCapabilitiesFullDuplex = 0x4, XtCapabilitiesChannelMask = 0x8, XtCapabilitiesXRunDetection = 0x10 } XtCapabilities;

#endif // XT_AUDIO_ENUMS_H