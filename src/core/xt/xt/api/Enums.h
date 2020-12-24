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

typedef enum XtDeviceCaps {
  XtDeviceCapsNone = 0x0, XtDeviceCapsInput = 0x1, XtDeviceCapsOutput = 0x2, XtDeviceCapsLoopback = 0x4, XtDeviceCapsHwDirect = 0x8
} XtDeviceCaps;

typedef enum XtServiceCaps {
  XtServiceCapsNone = 0x0, XtServiceCapsTime = 0x1, XtServiceCapsLatency = 0x2, XtServiceCapsFullDuplex = 0x4, 
  XtServiceCapsAggregation = 0x8, XtServiceCapsChannelMask = 0x10, XtServiceCapsControlPanel = 0x20, XtServiceCapsXRunDetection = 0x40
} XtServiceCaps;

#endif // XT_API_ENUMS_H