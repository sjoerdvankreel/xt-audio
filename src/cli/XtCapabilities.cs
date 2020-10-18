using System;

namespace Xt
{
    [Flags]
    public enum XtCapabilities : int
    {
        None = 0x0,
        Time = 0x1,
        Latency = 0x2,
        FullDuplex = 0x4,
        ChannelMask = 0x8,
        XRunDetection = 0x10
    }
}