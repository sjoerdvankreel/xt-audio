package xt.audio;

import com.sun.jna.Native;
import com.sun.jna.Pointer;

import xt.audio.Callbacks.XtOnError;
import xt.audio.Enums.XtSample;
import xt.audio.Structs.XtAttributes;
import xt.audio.Structs.XtErrorInfo;
import xt.audio.Structs.XtVersion;

public final class XtAudio {

    private static XtOnError _onError;
    static { Native.register(Utility.LIBRARY); }
    private static native XtVersion.ByValue XtAudioGetVersion();
    private static native void XtAudioSetOnError(XtOnError onError);
    private static native Pointer XtAudioInit(String id, Pointer window);
    private static native XtErrorInfo.ByValue XtAudioGetErrorInfo(long error);
    private static native XtAttributes.ByValue XtAudioGetSampleAttributes(XtSample sample);

    private XtAudio() {}

    public static XtVersion getVersion() { return XtAudioGetVersion(); }
    public static XtErrorInfo getErrorInfo(long error) { return XtAudioGetErrorInfo(error); }
    public static XtAttributes getSampleAttributes(XtSample sample) { return XtAudioGetSampleAttributes(sample); }
    public static XtPlatform init(String id, Pointer window) { return new XtPlatform(XtAudioInit(id, window)); }

    public static void setOnError(XtOnError onError)
    {
        _onError = onError;
        XtAudioSetOnError(onError);
    }
}