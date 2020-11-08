package com.xtaudio.xt;

import java.util.Arrays;
import java.util.EnumSet;

import com.sun.jna.Pointer;
import com.sun.jna.ptr.IntByReference;
import com.sun.jna.ptr.PointerByReference;
import static com.xtaudio.xt.XtNative.*;
import static com.xtaudio.xt.NativeTypes.*;

public final class XtService {

    private static native int XtServiceGetCapabilities(Pointer s);
    private static native long XtServiceGetDeviceCount(Pointer s, IntByReference count);
    private static native long XtServiceOpenDevice(Pointer s, int index, PointerByReference device);
    private static native long XtServiceOpenDefaultDevice(Pointer s, boolean output, PointerByReference device);
    private static native long XtServiceAggregateStream(Pointer s, Pointer[] devices,
                                                        XtChannels[] channels, double[] bufferSizes, int count, XtMix mix,
                                                        boolean interleaved, Pointer master, XtStreamCallback streamCallback,
                                                        XtXRunCallback xRunCallback, Pointer user, PointerByReference stream);
    private final Pointer _s;
    XtService(Pointer s) { _s = s; }

    public int getDeviceCount() {
        var count = new IntByReference();
        handleError(XtServiceGetDeviceCount(_s, count));
        return count.getValue();
    }

    public XtDevice openDevice(int index) {
        var d = new PointerByReference();
        handleError(XtServiceOpenDevice(_s, index, d));
        return new XtDevice(d.getValue());
    }

    public XtDevice openDefaultDevice(boolean output) {
        var d = new PointerByReference();
        handleError(XtServiceOpenDefaultDevice(_s, output, d));
        return d.getValue() == null? null: new XtDevice(d.getValue());
    }

    public EnumSet<XtCapabilities> getCapabilities() {
        var result = EnumSet.noneOf(XtCapabilities.class);
        var flags = XtServiceGetCapabilities(_s);
        if((flags & XtCapabilities.TIME._flag) != 0) result.add(XtCapabilities.TIME);
        if((flags & XtCapabilities.LATENCY._flag) != 0) result.add(XtCapabilities.LATENCY);
        if((flags & XtCapabilities.FULL_DUPLEX._flag) != 0) result.add(XtCapabilities.FULL_DUPLEX);
        if((flags & XtCapabilities.CHANNEL_MASK._flag) != 0) result.add(XtCapabilities.CHANNEL_MASK);
        if((flags & XtCapabilities.XRUN_DETECTION._flag) != 0) result.add(XtCapabilities.XRUN_DETECTION);
        return result;
    }

    public XtStream aggregateStream(XtDevice[] devices, XtChannels[] channels, double[] bufferSizes, int count, XtMix mix,
                                    boolean interleaved, XtDevice master, XtStreamCallback streamCallback, XtXRunCallback xRunCallback) {

        var stream = new PointerByReference();
        var handles = Arrays.stream(devices).map(d -> d.handle()).toArray(Pointer[]::new);
        handleError(XtServiceAggregateStream(_s, handles, channels, bufferSizes, count, mix, interleaved, master.handle(), streamCallback, xRunCallback, Pointer.NULL, stream));
        return new XtStream(stream.getValue(), streamCallback, xRunCallback);
    }
}