package xt.audio;

import com.sun.jna.Native;
import com.sun.jna.Pointer;
import com.sun.jna.ptr.IntByReference;
import xt.audio.CoreCallbacks.OnBuffer;
import xt.audio.CoreCallbacks.OnXRun;
import xt.audio.CoreCallbacks.XtOnBuffer;
import xt.audio.CoreCallbacks.XtOnXRun;
import xt.audio.CoreStructs.XtBuffer;
import xt.audio.CoreStructs.XtFormat;
import xt.audio.CoreStructs.XtLatency;
import static xt.audio.Utility.handleError;

public final class XtStream implements AutoCloseable {

    static { Native.register(Utility.LIBRARY); }
    private static native long XtStreamStop(Pointer s);
    private static native long XtStreamStart(Pointer s);
    private static native void XtStreamDestroy(Pointer s);
    private static native XtFormat XtStreamGetFormat(Pointer s);
    private static native long XtStreamGetLatency(Pointer s, XtLatency latency);
    private static native long XtStreamGetFrames(Pointer s, IntByReference frames);

    private Pointer _s;
    private XtFormat _format;

    private final Object _user;
    private final XtOnXRun _onXRun;
    private final XtOnBuffer _onBuffer;
    private final OnXRun _onNativeXRun;
    private final OnBuffer _onNativeBuffer;
    private final XtBuffer _buffer = new XtBuffer();
    private final XtLatency _latency = new XtLatency();
    private final IntByReference _frames = new IntByReference();

    public XtFormat getFormat() { return _format; }
    public void stop() { handleError(XtStreamStop(_s)); }
    @Override public void close() { XtStreamDestroy(_s); }
    public void start() { handleError(XtStreamStart(_s)); }

    OnXRun onNativeXRun() { return _onNativeXRun; }
    OnBuffer onNativeBuffer() { return _onNativeBuffer; }

    XtStream(XtOnBuffer onBuffer, XtOnXRun onXRun, Object user) {
        _user = user;
        _onXRun = onXRun;
        _onBuffer = onBuffer;
        _onNativeXRun = this::onXRun;
        _onNativeBuffer = this::onBuffer;
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

    private void onXRun(int index, Pointer user) throws Exception {
        _onXRun.callback(index, _user);
    }

    private void onBuffer(Pointer stream, Pointer buffer, Pointer user) throws Exception {
        for(int i = 0; i < Native.getNativeSize(XtBuffer.ByValue.class); i++)
            _buffer.getPointer().setByte(i, buffer.getByte(i));
        _buffer.read();
        _onBuffer.callback(this, _buffer, _user);
    }
}