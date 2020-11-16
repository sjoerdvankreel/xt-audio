package com.xtaudio.xt;

public interface CoreEnums {

    public enum XtSample { UINT8, INT16, INT24, INT32, FLOAT32 }
    public enum XtSetup { PRO_AUDIO, SYSTEM_AUDIO, CONSUMER_AUDIO }
    public enum XtCause { FORMAT, SERVICE, GENERIC, UNKNOWN, ENDPOINT }
    public enum XtSystem { ALSA, ASIO, JACK, WASAPI, PULSE_AUDIO, DIRECT_SOUND }

    public enum XtCapabilities {
        NONE(0x0), TIME(0x1), LATENCY(0x2), FULL_DUPLEX(0x4), CHANNEL_MASK(0x8), XRUN_DETECTION(0x10);
        final int _flag;
        private XtCapabilities(int flag) { _flag = flag; }
    }
}