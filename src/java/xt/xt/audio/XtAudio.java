package xt.audio;

import com.sun.jna.Native;
import com.sun.jna.Pointer;
import com.sun.jna.ptr.IntByReference;

import xt.audio.CoreCallbacks.XtOnError;
import xt.audio.CoreEnums.XtSample;
import xt.audio.CoreEnums.XtSetup;
import xt.audio.CoreEnums.XtSystem;
import xt.audio.CoreStructs.XtAttributes;
import xt.audio.CoreStructs.XtErrorInfo;
import xt.audio.CoreStructs.XtVersion;
import java.util.Arrays;

public final class XtAudio implements AutoCloseable {

    static { Native.register(Utility.LIBRARY); }
    private static native void XtAudioTerminate();
    private static native XtVersion.ByValue XtAudioGetVersion();
    private static native Pointer XtAudioGetService(XtSystem system);
    private static native XtSystem XtAudioSetupToSystem(XtSetup setup);
    private static native XtErrorInfo.ByValue XtAudioGetErrorInfo(long error);
    private static native void XtAudioGetSystems(int[] buffer, IntByReference size);
    private static native void XtAudioInit(String id, Pointer window, XtOnError onError);
    private static native XtAttributes.ByValue XtAudioGetSampleAttributes(XtSample sample);

    private XtAudio() {}
    static XtOnError _onError;

    @Override public void close() { XtAudioTerminate(); }
    public static XtVersion getVersion() { return XtAudioGetVersion(); }
    public static XtErrorInfo getErrorInfo(long error) { return XtAudioGetErrorInfo(error); }
    public static XtSystem setupToSystem(XtSetup setup) { return XtAudioSetupToSystem(setup); }
    public static XtAttributes getSampleAttributes(XtSample sample) { return XtAudioGetSampleAttributes(sample); }

    public static XtService getService(XtSystem system) {
        Pointer service = XtAudioGetService(system);
        return service == Pointer.NULL? null: new XtService(service);
    }

    public static AutoCloseable init(String id, Pointer window, XtOnError onError) {
        _onError = onError;
        XtAudioInit(id, window, onError);
        return new XtAudio();
    }

    public static XtSystem[] getSystems() {
        var mapper = new XtTypeMapper();
        var size = new IntByReference();
        XtAudioGetSystems(null, size);
        var result = new int[size.getValue()];
        XtAudioGetSystems(result, size);
        var converter = mapper.getFromNativeConverter(XtSystem.class);
        return Arrays.stream(result).mapToObj(s -> converter.fromNative(s, null)).toArray(XtSystem[]::new);
    }
}