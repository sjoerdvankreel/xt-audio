package com.xtaudio.xt;

import java.util.*;

import com.sun.jna.*;
import com.sun.jna.ptr.IntByReference;
import com.sun.jna.ptr.PointerByReference;
import static com.xtaudio.xt.XtNative.*;
import static com.xtaudio.xt.NativeTypes.*;

public final class XtService {

    static { Native.register(XtNative.getLibrary()); }
    private static native int XtServiceGetCapabilities(Pointer s);
    private static native long XtServiceGetDeviceCount(Pointer s, IntByReference count);
    private static native long XtServiceOpenDevice(Pointer s, int index, PointerByReference device);
    private static native long XtServiceOpenDefaultDevice(Pointer s, boolean output, PointerByReference device);
    private static native long XtServiceAggregateStream(Pointer s, AggregateStreamParams params, Pointer user, PointerByReference stream);

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

    static byte[] toNative(XtAggregateDeviceParams params) {
        var result = new AggregateDeviceParams();
        result.channels = params.channels;
        result.bufferSize = params.bufferSize;
        result.device = params.device.handle();
        result.write();
        return result.getPointer().getByteArray(0, result.size());
    }

    public XtStream aggregateStream(XtAggregateStreamParams params, Object user) {
        var stream = new PointerByReference();
        var native_ = new AggregateStreamParams();
        var size = Native.getNativeSize(AggregateDeviceParams.ByValue.class);
        var result = new XtStream(params.stream.onBuffer, params.stream.onXRun, user);
        var devices = new Memory(params.count * size);
        for(int i = 0; i < params.count; i++)
            devices.write(i * size, toNative(params.devices[i]), 0, size);
        native_.mix = params.mix;
        native_.devices = devices;
        native_.count = params.count;
        native_.stream = new StreamParams();
        native_.master = params.master.handle();
        native_.stream.onBuffer = result.onNativeBuffer();
        native_.stream.interleaved = params.stream.interleaved;
        native_.stream.onXRun = params.stream.onXRun == null? null: result.onNativeXRun();
        handleError(XtServiceAggregateStream(_s, native_, Pointer.NULL, stream));
        result.init(stream.getValue());
        return result;
    }
}