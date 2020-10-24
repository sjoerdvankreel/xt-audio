package com.xtaudio.xt;

import com.sun.jna.Pointer;
import com.sun.jna.ptr.IntByReference;
import com.sun.jna.ptr.PointerByReference;
import com.xtaudio.xt.NativeTypes.XtBuffer;
import com.xtaudio.xt.NativeTypes.XtFormat;
import com.xtaudio.xt.NativeTypes.XtMix;
import com.xtaudio.xt.NativeTypes.XtStreamCallback;
import com.xtaudio.xt.NativeTypes.XtSystem;
import com.xtaudio.xt.NativeTypes.XtXRunCallback;
import java.util.Optional;

public class XtDevice implements XtCloseable {

    Pointer d;

    XtDevice() {

    }

    XtDevice(Pointer d) {
        this.d = d;
    }

    @Override
    public String toString() {
        return getName();
    }

    public XtSystem getSystem() {
        return XtSystem.class.getEnumConstants()[XtNative.XtDeviceGetSystem(d) - 1];
    }

    public void showControlPanel() {
        XtNative.handleError(XtNative.XtDeviceShowControlPanel(d));
    }

    @Override
    public void close() {
        if (d != null)
            XtNative.XtDeviceDestroy(d);
        d = null;
    }

    public String getName() {
        PointerByReference name = new PointerByReference();
        XtNative.handleError(XtNative.XtDeviceGetName(d, name));
        return XtNative.wrapAndFreeString(name.getValue());
    }

    public int getChannelCount(boolean output) {
        IntByReference count = new IntByReference();
        XtNative.handleError(XtNative.XtDeviceGetChannelCount(d, output, count));
        return count.getValue();
    }

    public XtBuffer getBuffer(XtFormat format) {
        XtBuffer buffer = new XtBuffer();
        XtNative.handleError(XtNative.XtDeviceGetBuffer(d, XtNative.Format.toNative(format), buffer));
        return buffer;
    }

    public Optional<XtMix> getMix() {
        XtMix mix = new XtMix();
        IntByReference valid = new IntByReference();
        XtNative.handleError(XtNative.XtDeviceGetMix(d, valid, mix));
        return valid.getValue() == 0? Optional.empty(): Optional.of(mix);
    }

    public boolean supportsFormat(XtFormat format) {
        IntByReference supports = new IntByReference();
        XtNative.handleError(XtNative.XtDeviceSupportsFormat(d, XtNative.Format.toNative(format), supports));
        return supports.getValue() != 0;
    }

    public boolean supportsAccess(boolean interleaved) {
        IntByReference supports = new IntByReference();
        XtNative.handleError(XtNative.XtDeviceSupportsAccess(d, interleaved, supports));
        return supports.getValue() != 0;
    }

    public String getChannelName(boolean output, int index) {
        PointerByReference name = new PointerByReference();
        XtNative.handleError(XtNative.XtDeviceGetChannelName(d, output, index, name));
        return XtNative.wrapAndFreeString(name.getValue());
    }

    public XtStream openStream(XtFormat format, boolean interleaved, boolean raw, double bufferSize,
            XtStreamCallback streamCallback, XtXRunCallback xRunCallback, Object user) {

        PointerByReference s = new PointerByReference();
        XtStream stream = new XtStream(raw, streamCallback, xRunCallback, user);
        XtNative.Format formatNative = XtNative.Format.toNative(format);
        XtNative.handleError(XtNative.XtDeviceOpenStream(d, formatNative, interleaved, bufferSize,
                stream.nativeStreamCallback, xRunCallback == null ? null : stream.nativeXRunCallback, null, s));
        stream.init(s.getValue());
        return stream;
    }
}