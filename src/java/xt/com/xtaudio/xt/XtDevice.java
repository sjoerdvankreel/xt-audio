package com.xtaudio.xt;

import com.sun.jna.Native;
import com.sun.jna.Pointer;
import com.xtaudio.xt.CoreStructs.DeviceStreamParams;
import com.xtaudio.xt.CoreStructs.StreamParams;
import com.xtaudio.xt.CoreStructs.XtBufferSize;
import com.xtaudio.xt.CoreStructs.XtDeviceStreamParams;
import com.xtaudio.xt.CoreStructs.XtFormat;
import com.xtaudio.xt.CoreStructs.XtMix;
import java.nio.charset.Charset;
import java.util.Optional;

import com.sun.jna.ptr.IntByReference;
import com.sun.jna.ptr.PointerByReference;
import static com.xtaudio.xt.Utility.handleError;

public final class XtDevice implements AutoCloseable {

    static { Native.register(Utility.LIBRARY); }
    private static native void XtDeviceDestroy(Pointer d);
    private static native long XtDeviceShowControlPanel(Pointer d);
    private static native long XtDeviceGetMix(Pointer d, IntByReference valid, XtMix mix);
    private static native long XtDeviceGetName(Pointer d, byte[] buffer, IntByReference size);
    private static native long XtDeviceGetBufferSize(Pointer d, XtFormat format, XtBufferSize size);
    private static native long XtDeviceGetChannelCount(Pointer d, boolean output, IntByReference count);
    private static native long XtDeviceSupportsFormat(Pointer d, XtFormat format, IntByReference supports);
    private static native long XtDeviceSupportsAccess(Pointer d, boolean interleaved, IntByReference supports);
    private static native long XtDeviceGetChannelName(Pointer d, boolean output, int index, byte[] buffer, IntByReference size);
    private static native long XtDeviceOpenStream(Pointer d, DeviceStreamParams params, Pointer user, PointerByReference stream);

    private final Pointer _d;
    Pointer handle() { return _d; }
    XtDevice(Pointer d) { _d = d; }

    @Override public void close() { XtDeviceDestroy(_d); }
    @Override public String toString() { return getName(); }
    public void showControlPanel() { handleError(XtDeviceShowControlPanel(_d)); }

    public XtBufferSize getBufferSize(XtFormat format) {
        var result = new XtBufferSize();
        handleError(XtDeviceGetBufferSize(_d, format, result));
        return result;
    }

    public int getChannelCount(boolean output) {
        var count = new IntByReference();
        handleError(XtDeviceGetChannelCount(_d, output, count));
        return count.getValue();
    }

    public boolean supportsFormat(XtFormat format) {
        var supports = new IntByReference();
        handleError(XtDeviceSupportsFormat(_d, format, supports));
        return supports.getValue() != 0;
    }

    public boolean supportsAccess(boolean interleaved) {
        var supports = new IntByReference();
        handleError(XtDeviceSupportsAccess(_d, interleaved, supports));
        return supports.getValue() != 0;
    }

    public Optional<XtMix> getMix() {
        XtMix mix = new XtMix();
        var valid = new IntByReference();
        handleError(XtDeviceGetMix(_d, valid, mix));
        return valid.getValue() == 0? Optional.empty(): Optional.of(mix);
    }

    public String getName() {
        var size = new IntByReference();
        handleError(XtDeviceGetName(_d, null, size));
        byte[] buffer = new byte[size.getValue()];
        handleError(XtDeviceGetName(_d, buffer, size));
        return new String(buffer, 0, size.getValue() - 1, Charset.forName("UTF-8"));
    }

    public String getChannelName(boolean output, int index) {
        var size = new IntByReference();
        handleError(XtDeviceGetChannelName(_d, output, index, null, size));
        byte[] buffer = new byte[size.getValue()];
        handleError(XtDeviceGetChannelName(_d, output, index, buffer, size));
        return new String(buffer, 0, size.getValue() - 1, Charset.forName("UTF-8"));
    }

    public XtStream openStream(XtDeviceStreamParams params, Object user) {
        var stream = new PointerByReference();
        var result = new XtStream(params.stream.onBuffer, params.stream.onXRun, user);
        var native_ = new DeviceStreamParams();
        native_.format = params.format;
        native_.stream = new StreamParams();
        native_.bufferSize = params.bufferSize;
        native_.stream.onBuffer = result.onNativeBuffer();
        native_.stream.interleaved = params.stream.interleaved;
        native_.stream.onXRun = params.stream.onXRun == null? null: result.onNativeXRun();
        handleError(XtDeviceOpenStream(_d, native_, Pointer.NULL, stream));
        result.init(stream.getValue());
        return result;
    }
}