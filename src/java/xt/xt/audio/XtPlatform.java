package xt.audio;

import com.sun.jna.Native;
import com.sun.jna.Pointer;
import com.sun.jna.ptr.IntByReference;

import xt.audio.Enums.XtSetup;
import xt.audio.Enums.XtSystem;
import java.util.Arrays;

public final class XtPlatform implements AutoCloseable {

    static { Native.register(Utility.LIBRARY); }
    private static native void XtPlatformDestroy(Pointer p);
    private static native Pointer XtPlatformGetService(Pointer p, XtSystem system);
    private static native XtSystem XtPlatformSetupToSystem(Pointer p, XtSetup setup);
    private static native void XtPlatformGetSystems(Pointer p, int[] buffer, IntByReference size);

    Pointer _p;
    XtPlatform(Pointer p) { _p = p; }

    @Override public void close() { XtPlatformDestroy(_p); _p = Pointer.NULL; }
    public XtSystem setupToSystem(XtSetup setup) { return XtPlatformSetupToSystem(_p, setup); }

    public XtService getService(XtSystem system) {
        Pointer s = XtPlatformGetService(_p, system);
        return s == Pointer.NULL? null: new XtService(s);
    }

    public XtSystem[] getSystems() {
        var mapper = new XtTypeMapper();
        var size = new IntByReference();
        XtPlatformGetSystems(_p, null, size);
        var result = new int[size.getValue()];
        XtPlatformGetSystems(_p, result, size);
        var converter = mapper.getFromNativeConverter(XtSystem.class);
        return Arrays.stream(result).mapToObj(s -> converter.fromNative(s, null)).toArray(XtSystem[]::new);
    }
}