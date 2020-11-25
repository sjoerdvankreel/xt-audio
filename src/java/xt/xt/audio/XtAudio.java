package xt.audio;

import com.sun.jna.Native;
import com.sun.jna.Pointer;

import xt.audio.Callbacks.XtOnError;
import xt.audio.Enums.XtSample;
import xt.audio.Enums.XtSetup;
import xt.audio.Enums.XtSystem;
import xt.audio.Structs.XtAttributes;
import xt.audio.Structs.XtErrorInfo;
import xt.audio.Structs.XtVersion;

public final class XtAudio {

    static { Native.register(Utility.LIBRARY); }
    private static native XtVersion.ByValue XtAudioGetVersion();
    private static native XtSystem XtAudioSetupToSystem(XtSetup setup);
    private static native XtErrorInfo.ByValue XtAudioGetErrorInfo(long error);
    private static native XtAttributes.ByValue XtAudioGetSampleAttributes(XtSample sample);
    private static native Pointer XtAudioInit(String id, Pointer window, XtOnError onError);

    private XtAudio() {}
    public static XtVersion getVersion() { return XtAudioGetVersion(); }
    public static XtErrorInfo getErrorInfo(long error) { return XtAudioGetErrorInfo(error); }
    public static XtSystem setupToSystem(XtSetup setup) { return XtAudioSetupToSystem(setup); }
    public static XtAttributes getSampleAttributes(XtSample sample) { return XtAudioGetSampleAttributes(sample); }
    public static XtPlatform init(String id, Pointer window, XtOnError onError) { return new XtPlatform(XtAudioInit(id, window, onError), onError); }
}