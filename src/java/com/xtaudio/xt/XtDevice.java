package com.xtaudio.xt;

import java.nio.charset.Charset;
import java.util.Optional;

import com.sun.jna.Pointer;
import com.sun.jna.ptr.IntByReference;
import com.sun.jna.ptr.PointerByReference;
import static com.xtaudio.xt.XtNative.*;
import static com.xtaudio.xt.NativeTypes.*;

public final class XtDevice implements XtCloseable {

    private static native void XtDeviceDestroy(Pointer d);
    private static native long XtDeviceShowControlPanel(Pointer d);
    private static native long XtDeviceGetMix(Pointer d, IntByReference valid, XtMix mix);
    private static native long XtDeviceGetName(Pointer d, byte[] buffer, IntByReference size);
    private static native long XtDeviceGetBufferSize(Pointer d, XtFormat format, XtBufferSize size);
    private static native long XtDeviceGetChannelCount(Pointer d, boolean output, IntByReference count);
    private static native long XtDeviceSupportsFormat(Pointer d, XtFormat format, IntByReference supports);
    private static native long XtDeviceSupportsAccess(Pointer d, boolean interleaved, IntByReference supports);
    private static native long XtDeviceGetChannelName(Pointer d, boolean output, int index, byte[] buffer, IntByReference size);
    private static native long XtDeviceOpenStream(Pointer d, XtFormat format, boolean interleaved, double bufferSize,
                                                  XtStreamCallback streamCallback, XtXRunCallback xRunCallback,
                                                  Pointer user, PointerByReference stream);

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

    public XtStream openStream(XtFormat format, boolean interleaved, double bufferSize,
                               XtStreamCallback streamCallback, XtXRunCallback xRunCallback) {

        var stream = new PointerByReference();
        handleError(XtDeviceOpenStream(_d, format, interleaved, bufferSize, streamCallback, xRunCallback, Pointer.NULL, stream));
        return new XtStream(stream.getValue(), streamCallback, xRunCallback);
    }
}