package com.xtaudio.xt;

public final class XtPrint {

    private XtPrint() {
    }

    public static String errorToString(long error) {
        return XtNative.wrapAndFreeString(XtNative.XtPrintErrorToString(error));
    }

    public static String mixToString(XtMix mix) {
        return XtNative.wrapAndFreeString(XtNative.XtPrintMixToString(XtNative.Mix.toNative(mix)));
    }

    public static String levelToString(XtLevel level) {
        return XtNative.XtPrintLevelToString(level.ordinal());
    }

    public static String causeToString(XtCause cause) {
        return XtNative.XtPrintCauseToString(cause.ordinal());
    }

    public static String setupToString(XtSetup setup) {
        return XtNative.XtPrintSetupToString(setup.ordinal());
    }

    public static String systemToString(XtSystem system) {
        return XtNative.XtPrintSystemToString(system.ordinal() + 1);
    }

    public static String sampleToString(XtSample sample) {
        return XtNative.XtPrintSampleToString(sample.ordinal());
    }

    public static String formatToString(XtFormat format) {
        return XtNative.wrapAndFreeString(XtNative.XtPrintFormatToString(XtNative.Format.toNative(format)));
    }

    public static String bufferToString(XtBuffer buffer) {
        return XtNative.wrapAndFreeString(XtNative.XtPrintBufferToString(buffer));
    }

    public static String latencyToString(XtLatency latency) {
        return XtNative.wrapAndFreeString(XtNative.XtPrintLatencyToString(latency));
    }

    public static String channelsToString(XtChannels channels) {
        return XtNative.wrapAndFreeString(XtNative.XtPrintChannelsToString(channels));
    }

    public static String capabilitiesToString(int capabilities) {
        return XtNative.wrapAndFreeString(XtNative.XtPrintCapabilitiesToString(capabilities));
    }

    public static String attributesToString(XtAttributes attributes) {
        return XtNative.wrapAndFreeString(XtNative.XtPrintAttributesToString(attributes));
    }
}