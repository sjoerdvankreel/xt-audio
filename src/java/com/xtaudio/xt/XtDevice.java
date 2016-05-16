package com.xtaudio.xt;

import com.sun.jna.Pointer;
import com.sun.jna.ptr.IntByReference;
import com.sun.jna.ptr.PointerByReference;

/* Copyright (C) 2015-2016 Sjoerd van Kreel.
 *
 * This file is part of XT-Audio.
 *
 * XT-Audio is free software: you can redistribute it and/or modify it under the 
 * terms of the GNU Lesser General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later version.
 *
 * XT-Audio is distributed in the hope that it will be useful, but WITHOUT ANY 
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with XT-Audio. If not, see<http://www.gnu.org/licenses/>.
 */
public final class XtDevice implements XtCloseable {

    private Pointer d;

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

    public XtMix getMix() {
        PointerByReference mix = new PointerByReference();
        XtNative.handleError(XtNative.XtDeviceGetMix(d, mix));
        XtMix result = mix.getValue() == null ? null : new XtNative.Mix(mix.getValue()).fromNative();
        XtNative.XtAudioFree(mix.getValue());
        return result;
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

    public XtStream openStream(XtFormat format, double bufferSize, XtStreamCallback callback, Object user) {
        PointerByReference s = new PointerByReference();
        XtStream stream = new XtStream(callback, user);
        XtNative.Format formatNative = XtNative.Format.toNative(format);
        stream.nativeCallback = stream::callback;
        XtNative.handleError(XtNative.XtDeviceOpenStream(d, formatNative, bufferSize, stream.nativeCallback, Pointer.NULL, s));
        stream.init(s.getValue());
        return stream;
    }
}
