package xt.audio;

import com.sun.jna.Memory;
import com.sun.jna.Native;
import com.sun.jna.Pointer;
import java.nio.charset.Charset;
import xt.audio.Enums.XtCapabilities;
import xt.audio.Enums.XtEnumFlags;
import xt.audio.Structs.AggregateDeviceParams;
import xt.audio.Structs.AggregateStreamParams;
import com.sun.jna.ptr.IntByReference;
import com.sun.jna.ptr.PointerByReference;
import xt.audio.Structs.StreamParams;
import xt.audio.Structs.XtAggregateDeviceParams;
import xt.audio.Structs.XtAggregateStreamParams;
import static xt.audio.Utility.handleError;
import java.util.EnumSet;

public final class XtService {

    static { Native.register(Utility.LIBRARY); }
    private static native int XtServiceGetCapabilities(Pointer s);
    private static native long XtServiceOpenDevice(Pointer s, int index, PointerByReference device);
    private static native long XtServiceOpenDeviceList(Pointer s, int flags, PointerByReference list);
    private static native long XtServiceAggregateStream(Pointer s, AggregateStreamParams params, Pointer user, PointerByReference stream);
    private static native long XtServiceGetDefaultDeviceId(Pointer s, boolean output, IntByReference valid, byte[] buffer, IntByReference size);

    static byte[] toNative(XtAggregateDeviceParams params) {
        var result = new AggregateDeviceParams();
        result.channels = params.channels;
        result.bufferSize = params.bufferSize;
        result.device = params.device.handle();
        result.write();
        return result.getPointer().getByteArray(0, result.size());
    }

    private final Pointer _s;
    XtService(Pointer s) { _s = s; }

    public XtDevice openDevice(int index) {
        var d = new PointerByReference();
        handleError(XtServiceOpenDevice(_s, index, d));
        return new XtDevice(d.getValue());
    }

    public String getDefaultDeviceId(boolean output) {
        var size = new IntByReference();
        IntByReference valid = new IntByReference();
        handleError(XtServiceGetDefaultDeviceId(_s, output, valid, null, size));
        if(valid.getValue() == 0) return null;
        byte[] buffer = new byte[size.getValue()];
        handleError(XtServiceGetDefaultDeviceId(_s, output, valid, buffer, size));
        if(valid.getValue() == 0) return null;
        return new String(buffer, 0, size.getValue() - 1, Charset.forName("UTF-8"));
    }

    public XtDeviceList OpenDeviceList(EnumSet<XtEnumFlags> flags)
    {
        int flag = 0;
        for(XtEnumFlags f: flags) flag |= f._flag;
        PointerByReference list = new PointerByReference();
        return handleError(XtServiceOpenDeviceList(_s, flag, list), new XtDeviceList(list.getValue()));
    }

    public EnumSet<XtCapabilities> getCapabilities() {
        var result = EnumSet.noneOf(XtCapabilities.class);
        var flags = XtServiceGetCapabilities(_s);
        if((flags & XtCapabilities.TIME._flag) != 0) result.add(XtCapabilities.TIME);
        if((flags & XtCapabilities.LATENCY._flag) != 0) result.add(XtCapabilities.LATENCY);
        if((flags & XtCapabilities.FULL_DUPLEX._flag) != 0) result.add(XtCapabilities.FULL_DUPLEX);
        if((flags & XtCapabilities.AGGREGATION._flag) != 0) result.add(XtCapabilities.AGGREGATION);
        if((flags & XtCapabilities.CHANNEL_MASK._flag) != 0) result.add(XtCapabilities.CHANNEL_MASK);
        if((flags & XtCapabilities.XRUN_DETECTION._flag) != 0) result.add(XtCapabilities.XRUN_DETECTION);
        return result;
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