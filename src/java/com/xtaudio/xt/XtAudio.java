package com.xtaudio.xt;

import com.sun.jna.Native;
import com.sun.jna.Pointer;
import com.xtaudio.xt.NativeTypes.XtAttributes;
import com.xtaudio.xt.NativeTypes.XtCause;
import com.xtaudio.xt.NativeTypes.XtFatalCallback;
import com.xtaudio.xt.NativeTypes.XtLevel;
import com.xtaudio.xt.NativeTypes.XtSample;
import com.xtaudio.xt.NativeTypes.XtSetup;
import com.xtaudio.xt.NativeTypes.XtSystem;
import com.xtaudio.xt.NativeTypes.XtTraceCallback;
import com.xtaudio.xt.NativeTypes.XtVersion;
import java.util.ArrayList;
import java.util.List;

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

    public static XtVersion getVersion() {
        return XtNative.XtAudioGetVersion();
    }

    public static int getServiceCount() {
        return XtNative.XtAudioGetServiceCount();
    }

    public static XtCause getErrorCause(long error) {
        return XtCause.class.getEnumConstants()[XtNative.XtAudioGetErrorCause(error)];
    }

    public static XtSystem getErrorSystem(long error) {
        return XtSystem.class.getEnumConstants()[XtNative.XtAudioGetErrorSystem(error) - 1];
    }

    public static int getErrorFault(long error) {
        return XtNative.XtAudioGetErrorFault(error);
    }

    public static String getErrorText(long error) {
        return XtNative.XtAudioGetErrorText(error);
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

    public static List<String> printCapabilitiesToString(int capabilities) {
        int i = 0;
        List<String> result = new ArrayList<>();
        Pointer strings = XtNative.XtAudioPrintCapabilitiesToString(capabilities);
        while(strings.getPointer(i * Native.POINTER_SIZE) != null)
            result.add(strings.getPointer(i++ * Native.POINTER_SIZE).getString(0));
        return result;
    }
}