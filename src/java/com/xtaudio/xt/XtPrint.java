package com.xtaudio.xt;

import com.sun.jna.Native;
import com.sun.jna.Pointer;
import java.util.ArrayList;
import java.util.List;

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

    public static List<String> capabilitiesToString(int capabilities) {
        int i = 0;
        List<String> result = new ArrayList<>();
        Pointer strings = XtNative.XtPrintCapabilitiesToString(capabilities);
        while(strings.getPointer(i * Native.POINTER_SIZE) != null)
            result.add(strings.getPointer(i++ * Native.POINTER_SIZE).getString(0));
        return result;
    }
}