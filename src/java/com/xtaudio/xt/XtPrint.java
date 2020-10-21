package com.xtaudio.xt;

public final class XtPrint {

    private XtPrint() {
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

    public static String capabilitiesToString(int capabilities) {
        return XtNative.wrapAndFreeString(XtNative.XtPrintCapabilitiesToString(capabilities));
    }
}