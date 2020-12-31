#ifndef XT_API_ENUMS_H
#define XT_API_ENUMS_H

enum XtSetup { XtSetupProAudio, XtSetupSystemAudio, XtSetupConsumerAudio };
enum XtSample { XtSampleUInt8, XtSampleInt16, XtSampleInt24, XtSampleInt32, XtSampleFloat32 };
enum XtCause { XtCauseFormat, XtCauseService, XtCauseGeneric, XtCauseUnknown, XtCauseEndpoint };
enum XtSystem { XtSystemALSA = 1, XtSystemASIO, XtSystemJACK, XtSystemWASAPI, XtSystemPulse, XtSystemDSound };
enum XtEnumFlags { XtEnumFlagsInput = 0x1, XtEnumFlagsOutput = 0x2, XtEnumFlagsAll = XtEnumFlagsInput | XtEnumFlagsOutput };
enum XtDeviceCaps { XtDeviceCapsNone = 0x0, XtDeviceCapsInput = 0x1, XtDeviceCapsOutput = 0x2, XtDeviceCapsLoopback = 0x4, XtDeviceCapsHwDirect = 0x8 };
enum XtServiceCaps {
  XtServiceCapsNone = 0x0, XtServiceCapsTime = 0x1, XtServiceCapsLatency = 0x2, XtServiceCapsFullDuplex = 0x4, 
  XtServiceCapsAggregation = 0x8, XtServiceCapsChannelMask = 0x10, XtServiceCapsControlPanel = 0x20, XtServiceCapsXRunDetection = 0x40
};

/** @cond */
typedef enum XtSetup XtSetup;
typedef enum XtCause XtCause;
typedef enum XtSample XtSample;
typedef enum XtSystem XtSystem;
typedef enum XtEnumFlags XtEnumFlags;
typedef enum XtDeviceCaps XtDeviceCaps;
typedef enum XtServiceCaps XtServiceCaps;
/** @endcond */

#endif // XT_API_ENUMS_H