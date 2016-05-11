package com.xtaudio.xt;

import com.sun.jna.Pointer;

public final class XtAudio implements XtCloseable {

    static XtTraceCallback trace;
    private static XtFatalCallback fatal;
    private static XtNative.TraceCallback nativeTrace;

    private static void ForwardTrace(int level, String message) {
        if (trace != null)
            trace.callback(XtLevel.class.getEnumConstants()[level], message);
    }

    public XtAudio(String id, Pointer window, XtTraceCallback trace, XtFatalCallback fatal) {
        XtAudio.trace = trace;
        XtAudio.fatal = fatal;
        XtAudio.nativeTrace = XtAudio::ForwardTrace;
        XtNative.init();
        XtNative.XtAudioInit(id, window, nativeTrace, fatal);
    }

    @Override
    public void close() {
        XtNative.XtAudioTerminate();
    }

    public static boolean isWin32() {
        return XtNative.XtAudioIsWin32();
    }

    public static String getVersion() {
        return XtNative.XtAudioGetVersion();
    }

    public static int getServiceCount() {
        return XtNative.XtAudioGetServiceCount();
    }

    public static XtService getServiceByIndex(int index) {
        return new XtService(XtNative.XtAudioGetServiceByIndex(index));
    }

    public static XtService getServiceBySetup(XtSetup setup) {
        return new XtService(XtNative.XtAudioGetServiceBySetup(setup.ordinal()));
    }

    public static XtService getServiceBySystem(XtSystem system) {
        return new XtService(XtNative.XtAudioGetServiceBySystem(system.ordinal() + 1));
    }

    public static XtAttributes getSampleAttributes(XtSample sample) {
        XtAttributes result = new XtAttributes();
        XtNative.XtAudioGetSampleAttributes(sample.ordinal(), result);
        return result;
    }
}
