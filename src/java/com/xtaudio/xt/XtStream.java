package com.xtaudio.xt;

import com.sun.jna.*;
import com.sun.jna.ptr.IntByReference;
import static com.xtaudio.xt.XtNative.*;
import static com.xtaudio.xt.NativeTypes.*;

public final class XtStream implements XtCloseable {

    static { Native.register(XtNative.getLibrary()); }
    private static native long XtStreamStop(Pointer s);
    private static native long XtStreamStart(Pointer s);
    private static native void XtStreamDestroy(Pointer s);
    private static native XtFormat XtStreamGetFormat(Pointer s);
    private static native long XtStreamGetLatency(Pointer s, XtLatency latency);
    private static native long XtStreamGetFrames(Pointer s, IntByReference frames);

    private Pointer _s;
    Pointer handle() { return _s; }
    private final XtXRunCallback _xRunCallback;
    private StreamCallback _nativeStreamCallback;
    private final XtStreamCallback _streamCallback;
    private final XtBuffer _buffer = new XtBuffer();
    StreamCallback nativeStreamCallback() { return _nativeStreamCallback; }

    void init(Pointer s) { _s = s; }
    public void stop() { handleError(XtStreamStop(_s)); }
    @Override public void close() { XtStreamDestroy(_s); }
    public void start() { handleError(XtStreamStart(_s)); }
    public XtFormat getFormat() {return XtStreamGetFormat(_s); }

    XtStream(XtStreamCallback streamCallback, XtXRunCallback xRunCallback) {
        _xRunCallback = xRunCallback;
        _streamCallback = streamCallback;
        _nativeStreamCallback = this::streamCallback;
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

    private void streamCallback(Pointer stream, Pointer buffer, Pointer user) throws Exception {
        for(int i = 0; i < Native.getNativeSize(XtBuffer.ByValue.class); i++)
            _buffer.getPointer().setByte(i, buffer.getByte(i));
        _buffer.read();
        _streamCallback.callback(stream, _buffer, user);
    }
}