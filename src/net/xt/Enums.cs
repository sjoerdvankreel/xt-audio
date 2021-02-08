using System;

namespace Xt
{
    public enum XtSetup : int { ProAudio, SystemAudio, ConsumerAudio }
    public enum XtSample : int { UInt8, Int16, Int24, Int32, Float32 }
    public enum XtCause : int { Format, Service, Generic, Unknown, Endpoint }
    public enum XtSystem : int { ALSA = 1, ASIO, JACK, WASAPI, PulseAudio, DirectSound }
    [Flags] public enum XtEnumFlags { Input = 0x1, Output = 0x2, All = Input | Output }
    [Flags] public enum XtDeviceCaps { None = 0x0, Input = 0x1, Output = 0x2, Loopback = 0x4, HwDirect = 0x8 };
    [Flags] public enum XtServiceCaps : int { None = 0x0, Time = 0x1, Latency = 0x2, FullDuplex = 0x4, 
        Aggregation = 0x8, ChannelMask = 0x10, ControlPanel = 0x20, XRunDetection = 0x40 }
}