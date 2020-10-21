package com.xtaudio.xt;

public final class XtCapabilities {

    private XtCapabilities() {
    }

    public static final int NONE = 0x0;
    public static final int TIME = 0x1;
    public static final int LATENCY = 0x2;
    public static final int FULL_DUPLEX = 0x4;
    public static final int CHANNEL_MASK = 0x8;
    public static final int XRUN_DETECTION = 0x10;
    
    public static String toString(int capabilities) {
        return String.join(", ", XtAudio.printCapabilitiesToString(capabilities));
    }
}