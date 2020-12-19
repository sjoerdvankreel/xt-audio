#ifndef XT_API_ENUMS_H
#define XT_API_ENUMS_H

typedef enum XtSetup { 
  XtSetupProAudio, XtSetupSystemAudio, XtSetupConsumerAudio 
} XtSetup;

typedef enum XtSample { 
  XtSampleUInt8, XtSampleInt16, XtSampleInt24, XtSampleInt32, XtSampleFloat32 
} XtSample;

typedef enum XtCause { 
  XtCauseFormat, XtCauseService, XtCauseGeneric, XtCauseUnknown, XtCauseEndpoint
} XtCause;

typedef enum XtSystem {
  XtSystemALSA = 1, XtSystemASIO, XtSystemJACK, XtSystemWASAPI, XtSystemPulse, XtSystemDSound
} XtSystem;

typedef enum XtEnumFlags {
  XtEnumFlagsInput = 0x1, XtEnumFlagsOutput = 0x2, XtEnumFlagsAll = XtEnumFlagsInput | XtEnumFlagsOutput
} XtEnumFlags;

typedef enum XtCapabilities {
  XtCapabilitiesNone = 0x0, XtCapabilitiesTime = 0x1, XtCapabilitiesLatency = 0x2, XtCapabilitiesFullDuplex = 0x4, 
  XtCapabilitiesAggregation = 0x8, XtCapabilitiesChannelMask = 0x10, XtCapabilitiesControlPanel = 0x20, XtCapabilitiesXRunDetection = 0x40
} XtCapabilities;

#endif // XT_API_ENUMS_H