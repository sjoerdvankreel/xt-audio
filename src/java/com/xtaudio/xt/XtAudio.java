package com.xtaudio.xt;

import com.sun.jna.Pointer;
import com.xtaudio.xt.NativeTypes.XtAttributes;
import com.xtaudio.xt.NativeTypes.XtErrorInfo;
import com.xtaudio.xt.NativeTypes.XtFatalCallback;
import com.xtaudio.xt.NativeTypes.XtLevel;
import com.xtaudio.xt.NativeTypes.XtSample;
import com.xtaudio.xt.NativeTypes.XtSetup;
import com.xtaudio.xt.NativeTypes.XtSystem;
import com.xtaudio.xt.NativeTypes.XtTraceCallback;
import com.xtaudio.xt.NativeTypes.XtVersion;

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

    public static XtVersion getVersion() {
        return XtNative.XtAudioGetVersion();
    }

    public static int getServiceCount() {
        return XtNative.XtAudioGetServiceCount();
    }

    public static XtErrorInfo getErrorInfo(long error) {
        return XtNative.XtAudioGetErrorInfo(error);
    }
    
    public static XtService getServiceByIndex(int index) {
        return new XtService(XtNative.XtAudioGetServiceByIndex(index));
    }

    public static XtService getServiceBySetup(XtSetup setup) {
        Pointer service = XtNative.XtAudioGetServiceBySetup(setup.ordinal());
        return service == Pointer.NULL ? null : new XtService(service);
    }

    public static XtService getServiceBySystem(XtSystem system) {
        Pointer service = XtNative.XtAudioGetServiceBySystem(system.ordinal() + 1);
        return service == Pointer.NULL ? null : new XtService(service);
    }

    public static XtAttributes getSampleAttributes(XtSample sample) {
        return XtNative.XtAudioGetSampleAttributes(sample.ordinal());
    }
}