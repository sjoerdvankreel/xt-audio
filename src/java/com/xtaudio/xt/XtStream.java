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
    private XtFormat _format;
    private XRunCallback _nativeXRunCallback;
    private StreamCallback _nativeStreamCallback;

    private final Object _user;
    private final XtXRunCallback _xRunCallback;
    private final XtStreamCallback _streamCallback;
    private final XtBuffer _buffer = new XtBuffer();
    private final XtLatency _latency = new XtLatency();
    private final IntByReference _frames = new IntByReference();

    public XtFormat getFormat() { return _format; }
    public void stop() { handleError(XtStreamStop(_s)); }
    @Override public void close() { XtStreamDestroy(_s); }
    public void start() { handleError(XtStreamStart(_s)); }

    XRunCallback nativeXRunCallback() { return _nativeXRunCallback; }
    StreamCallback nativeStreamCallback() { return _nativeStreamCallback; }

    XtStream(XtStreamCallback streamCallback, XtXRunCallback xRunCallback, Object user) {
        _user = user;
        _xRunCallback = xRunCallback;
        _streamCallback = streamCallback;
        _nativeXRunCallback = this::xRunCallback;
        _nativeStreamCallback = this::streamCallback;
    }

    void init(Pointer s) {
        _s = s;
        _format = XtStreamGetFormat(_s);
    }

    public int getFrames() {
        handleError(XtStreamGetFrames(_s, _frames));
        return _frames.getValue();
    }

    public XtLatency getLatency() {
        handleError(XtStreamGetLatency(_s, _latency));
        return _latency;
    }

    private void xRunCallback(int index, Pointer user) throws Exception {
        _xRunCallback.callback(index, _user);
    }

    private void streamCallback(Pointer stream, Pointer buffer, Pointer user) throws Exception {
        for(int i = 0; i < Native.getNativeSize(XtBuffer.ByValue.class); i++)
            _buffer.getPointer().setByte(i, buffer.getByte(i));
        _buffer.read();
        _streamCallback.callback(this, _buffer, _user);
    }
}