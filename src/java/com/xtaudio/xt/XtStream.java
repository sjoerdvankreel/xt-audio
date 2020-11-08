package com.xtaudio.xt;

import com.sun.jna.Pointer;
import com.sun.jna.ptr.IntByReference;
import static com.xtaudio.xt.XtNative.*;
import static com.xtaudio.xt.NativeTypes.*;

public final class XtStream implements XtCloseable {

    private static native long XtStreamStop(Pointer s);
    private static native long XtStreamStart(Pointer s);
    private static native void XtStreamDestroy(Pointer s);
    private static native XtFormat.ByValue XtStreamGetFormat(Pointer s);
    private static native long XtStreamGetLatency(Pointer s, XtLatency latency);
    private static native long XtStreamGetFrames(Pointer s, IntByReference frames);

    private final Pointer _s;
    Pointer handle() { return _s; }
    private final XtXRunCallback _xRunCallback;
    private final XtStreamCallback _streamCallback;

    public void stop() { handleError(XtStreamStop(_s)); }
    @Override public void close() { XtStreamDestroy(_s); }
    public void start() { handleError(XtStreamStart(_s)); }
    public XtFormat getFormat() {return XtStreamGetFormat(_s); }

    XtStream(Pointer s, XtStreamCallback streamCallback, XtXRunCallback xRunCallback) {
        _s = s;
        _xRunCallback = xRunCallback;
        _streamCallback = streamCallback;
    }

    public int getFrames() {
        var frames = new IntByReference();
        handleError(XtStreamGetFrames(_s, frames));
        return frames.getValue();
    }

    public XtLatency getLatency() {
        var latency = new XtLatency();
        handleError(XtStreamGetLatency(_s, latency));
        return latency;
    }
}