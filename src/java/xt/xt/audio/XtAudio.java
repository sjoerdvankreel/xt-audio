package xt.audio;

import com.sun.jna.Native;
import com.sun.jna.Platform;
import com.sun.jna.Pointer;

import xt.audio.Callbacks.XtOnError;
import xt.audio.Enums.XtSample;
import xt.audio.Structs.XtAttributes;
import xt.audio.Structs.XtErrorInfo;
import xt.audio.Structs.XtVersion;
import xt.audio.NativeCallbacks.NativeOnError;
import static xt.audio.Utility.handleAssert;

public final class XtAudio {

    private static XtOnError _onError;
    private static NativeOnError _onNativeError;
    private static void onError(String message) throws Exception { _onError.callback(message); }

    static { Native.register(Utility.LIBRARY); }

    private static native XtVersion.ByValue XtAudioGetVersion();
    private static native void XtAudioSetOnError(NativeOnError onError);
    private static native Pointer XtAudioInit(String id, Pointer window);
    private static native XtErrorInfo.ByValue XtAudioGetErrorInfo(long error);
    private static native XtAttributes.ByValue XtAudioGetSampleAttributes(XtSample sample);

    private XtAudio() {}

    public static XtVersion getVersion() { return handleAssert(XtAudioGetVersion()); }
    public static XtErrorInfo getErrorInfo(long error) { return handleAssert(XtAudioGetErrorInfo(error)); }
    public static XtPlatform init(String id, Pointer window) { return new XtPlatform(handleAssert(XtAudioInit(id, window))); }
    public static XtAttributes getSampleAttributes(XtSample sample) { return handleAssert(XtAudioGetSampleAttributes(sample)); }

    public static void setOnError(XtOnError onError) {
        _onError = onError;
        _onNativeError = (NativeOnError)XtAudio::onError;
        handleAssert(() -> XtAudioSetOnError(_onNativeError));
    }
}