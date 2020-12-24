package xt.audio;

import com.sun.jna.Native;
import com.sun.jna.Pointer;
import com.sun.jna.ptr.IntByReference;
import java.nio.charset.Charset;
import static xt.audio.Utility.handleError;

public final class XtDeviceList implements AutoCloseable {

    static { Native.register(Utility.LIBRARY); }
    private static native void XtDeviceListDestroy(Pointer l);
    private static native Pointer XtDeviceListGetHandle(Pointer d);
    private static native long XtDeviceListGetCount(Pointer l, IntByReference count);
    private static native long XtDeviceListGetId(Pointer l, int index, byte[] buffer, IntByReference size);
    private static native long XtDeviceListGetName(Pointer l, String id, byte[] buffer, IntByReference size);

    private final Pointer _l;
    XtDeviceList(Pointer l) { _l = l; }

    @Override public void close() { XtDeviceListDestroy(_l); }
    public Pointer getHandle() { return XtDeviceListGetHandle(_l); }

    public int getCount()
    {
        var count = new IntByReference();
        handleError(XtDeviceListGetCount(_l, count));
        return count.getValue();
    }

    public String getId(int index)
    {
        var size = new IntByReference();
        handleError(XtDeviceListGetId(_l, index, null, size));
        byte[] buffer = new byte[size.getValue()];
        handleError(XtDeviceListGetId(_l, index, buffer, size));
        return new String(buffer, 0, size.getValue() - 1, Charset.forName("UTF-8"));
    }

    public String getName(String id)
    {
        var size = new IntByReference();
        handleError(XtDeviceListGetName(_l, id, null, size));
        byte[] buffer = new byte[size.getValue()];
        handleError(XtDeviceListGetName(_l, id, buffer, size));
        return new String(buffer, 0, size.getValue() - 1, Charset.forName("UTF-8"));
    }
}