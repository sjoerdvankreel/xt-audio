package xt.audio;

import com.sun.jna.Native;
import com.sun.jna.Platform;
import com.sun.jna.Pointer;
import com.sun.jna.ptr.IntByReference;
import xt.audio.Structs.XtBuffer;
import xt.audio.Structs.XtFormat;
import xt.audio.Structs.XtLatency;
import xt.audio.Structs.XtStreamParams;
import xt.audio.NativeCallbacks.NativeOnBuffer;
import xt.audio.NativeCallbacks.NativeOnRunning;
import xt.audio.NativeCallbacks.NativeOnXRun;
import xt.audio.NativeCallbacks.WinX86NativeOnBuffer;
import xt.audio.NativeCallbacks.WinX86NativeOnRunning;
import xt.audio.NativeCallbacks.WinX86NativeOnXRun;
import static xt.audio.Utility.handleAssert;
import static xt.audio.Utility.handleError;

public final class XtStream implements AutoCloseable {

    static { Native.register(Utility.LIBRARY); }
    private static native void XtStreamStop(Pointer s);
    private static native long XtStreamStart(Pointer s);
    private static native void XtStreamDestroy(Pointer s);
    private static native Pointer XtStreamGetHandle(Pointer s);
    private static native boolean XtStreamIsRunning(Pointer s);
    private static native XtFormat XtStreamGetFormat(Pointer s);
    private static native long XtStreamGetLatency(Pointer s, XtLatency latency);
    private static native long XtStreamGetFrames(Pointer s, IntByReference frames);

    private Pointer _s;
    private XtFormat _format;

    private final Object _user;
    private final XtStreamParams _params;
    private final NativeOnXRun _onNativeXRun;
    private final NativeOnBuffer _onNativeBuffer;
    private final NativeOnRunning _onNativeRunning;
    private final XtBuffer _buffer = new XtBuffer();
    private final XtLatency _latency = new XtLatency();
    private final IntByReference _frames = new IntByReference();

    public XtFormat getFormat() { return _format; }
    public void start() { handleError(XtStreamStart(_s)); }
    public void stop() { handleAssert(() -> XtStreamStop(_s));}
    public Pointer getHandle() { return handleAssert(XtStreamGetHandle(_s)); }
    public boolean isRunning() { return handleAssert(XtStreamIsRunning(_s)); }
    @Override public void close() { handleAssert(() -> XtStreamDestroy(_s)); _s = Pointer.NULL; }

    NativeOnXRun onNativeXRun() { return _onNativeXRun; }
    NativeOnBuffer onNativeBuffer() { return _onNativeBuffer; }
    NativeOnRunning onNativeRunning() { return _onNativeRunning; }

    XtStream(XtStreamParams params, Object user) {
        _user = user;
        _params = params;
        boolean stdcall = Platform.isWindows() && !Platform.is64Bit();
        _onNativeXRun = stdcall? (WinX86NativeOnXRun) this::onXRun: (NativeOnXRun)this::onXRun;
        _onNativeBuffer = stdcall? (WinX86NativeOnBuffer) this::onBuffer: (NativeOnBuffer)this::onBuffer;
        _onNativeRunning = stdcall? (WinX86NativeOnRunning) this::onRunning: (NativeOnRunning)this::onRunning;
    }

    void init(Pointer s) {
        _s = s;
        _format = handleAssert(XtStreamGetFormat(_s));
    }

    public int getFrames() {
        handleError(XtStreamGetFrames(_s, _frames));
        return _frames.getValue();
    }

    public XtLatency getLatency() {
        handleError(XtStreamGetLatency(_s, _latency));
        return _latency;
    }

    private void onXRun(Pointer stream, int index, Pointer user) throws Exception {
        _params.onXRun.callback(this, index, _user);
    }

    private int onBuffer(Pointer stream, Pointer buffer, Pointer user) throws Exception {
        for(int i = 0; i < Native.getNativeSize(XtBuffer.ByValue.class); i++)
            _buffer.getPointer().setByte(i, buffer.getByte(i));
        _buffer.read();
        return _params.onBuffer.callback(this, _buffer, _user);
    }

    private void onRunning(Pointer stream, boolean running, long error, Object user) throws Exception {
        _params.onRunning.callback(this, running, error, user);
    }
}