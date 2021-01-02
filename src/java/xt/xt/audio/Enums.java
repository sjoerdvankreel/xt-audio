package xt.audio;

public interface Enums {

    public enum XtSample { UINT8, INT16, INT24, INT32, FLOAT32 }
    public enum XtSetup { PRO_AUDIO, SYSTEM_AUDIO, CONSUMER_AUDIO }
    public enum XtCause { FORMAT, SERVICE, GENERIC, UNKNOWN, ENDPOINT }
    public enum XtSystem { ALSA, ASIO, JACK, WASAPI, PULSE_AUDIO, DIRECT_SOUND }

    public enum XtEnumFlags {
        INPUT(0x1), OUTPUT(0x2), ALL(0x1|0x2);
        final int _flag;
        private XtEnumFlags(int flag) { _flag = flag; }
    }

    public enum XtDeviceCaps {
        NONE(0x0), INPUT(0x1), OUTPUT(0x2), LOOPBACK(0x4), HW_DIRECT(0x8);
        final int _flag;
        private XtDeviceCaps(int flag) { _flag = flag; }
    }

    public enum XtServiceCaps {
        NONE(0x0), TIME(0x1), LATENCY(0x2), FULL_DUPLEX(0x4), AGGREGATION(0x8), CHANNEL_MASK(0x10), CONTROL_PANEL(0x20), XRUN_DETECTION(0x40);
        final int _flag;
        private XtServiceCaps(int flag) { _flag = flag; }
    }
}