using System;

namespace Xt
{
    public enum XtSetup : int { ProAudio, SystemAudio, ConsumerAudio }
    public enum XtSample : int { UInt8, Int16, Int24, Int32, Float32 }
    public enum XtCause : int { Format, Service, Generic, Unknown, Endpoint }
    public enum XtSystem : int { ALSA = 1, ASIO, JACK, WASAPI, PulseAudio, DirectSound }
    [Flags] public enum XtCapabilities : int { None = 0x0, Time = 0x1, Latency = 0x2, FullDuplex = 0x4, ChannelMask = 0x8, XRunDetection = 0x10 }
}