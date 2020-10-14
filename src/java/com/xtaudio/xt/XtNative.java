/* Copyright (C) 2015-2020 Sjoerd van Kreel.
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
package com.xtaudio.xt;

import com.sun.jna.Callback;
import com.sun.jna.Library;
import com.sun.jna.Native;
import com.sun.jna.NativeLibrary;
import com.sun.jna.Pointer;
import com.sun.jna.Structure;
import com.sun.jna.ptr.IntByReference;
import com.sun.jna.ptr.PointerByReference;
import com.sun.jna.win32.StdCallFunctionMapper;
import com.sun.jna.win32.StdCallLibrary;
import java.util.Arrays;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

final class XtNative {

    private static boolean initialized = false;

    private XtNative() {
    }

    public static class Mix extends Structure {

        public int rate;
        public int sample;

        Mix() {
        }

        Mix(Pointer value) {
            super(value);
            read();
        }

        @Override
        protected List getFieldOrder() {
            return Arrays.asList("rate", "sample");
        }

        XtMix fromNative() {
            XtMix result = new XtMix();
            result.rate = rate;
            result.sample = XtSample.class.getEnumConstants()[sample];
            return result;
        }

        static Mix toNative(XtMix mix) {
            Mix result = new Mix();
            result.rate = mix.rate;
            result.sample = mix.sample.ordinal();
            return result;
        }
    }

    public static class Format extends Structure {

        public int rate;
        public int sample;
        public int inputs;
        public long inMask;
        public int outputs;
        public long outMask;

        Format() {
        }

        Format(Pointer value) {
            super(value);
            read();
        }

        @Override
        protected List getFieldOrder() {
            return Arrays.asList("rate", "sample", "inputs", "inMask", "outputs", "outMask");
        }

        XtFormat fromNative() {
            XtFormat result = new XtFormat();
            result.mix.rate = rate;
            result.mix.sample = XtSample.values()[sample];
            result.channels.inputs = inputs;
            result.channels.inMask = inMask;
            result.channels.outputs = outputs;
            result.channels.outMask = outMask;
            return result;
        }

        static Format toNative(XtFormat format) {
            Format result = new Format();
            result.rate = format.mix.rate;
            result.sample = format.mix.sample.ordinal();
            result.inputs = format.channels.inputs;
            result.inMask = format.channels.inMask;
            result.outputs = format.channels.outputs;
            result.outMask = format.channels.outMask;
            return result;
        }
    }

    public static class ChannelsByValue extends XtChannels implements Structure.ByValue {
    }

    static interface XRunCallback extends Callback {

        void callback(int index, Pointer user);
    }

    static interface TraceCallback extends Callback {

        void callback(int level, String message);
    }

    static interface StreamCallback extends Callback {

        void callback(Pointer stream, Pointer input, Pointer output, int frames,
                double time, long position, boolean timeValid, long error, Pointer user);
    }

    static void init() {
        if (initialized)
            return;
        boolean isX64 = Native.POINTER_SIZE == 8;
        System.setProperty("jna.encoding", "UTF-8");
        boolean isWin32 = System.getProperty("os.name").contains("Windows");
        Map<String, Object> options = new HashMap<>();
        if (isWin32 && !isX64) {
            options.put(Library.OPTION_FUNCTION_MAPPER, new StdCallFunctionMapper());
            options.put(Library.OPTION_CALLING_CONVENTION, StdCallLibrary.STDCALL_CONVENTION);
        }
        if (isWin32 && !isX64)
            Native.register(NativeLibrary.getInstance("win32-x86/xt-core.dll", options));
        else if (isWin32 && isX64)
            Native.register(NativeLibrary.getInstance("win32-x64/xt-core.dll", options));
        else if (!isWin32 && !isX64)
            Native.register(NativeLibrary.getInstance("linux-x86/libxt-core.so", options));
        else
            Native.register(NativeLibrary.getInstance("linux-x64/libxt-core.so", options));
        initialized = true;
    }

    static void handleError(long error) {
        if (error != 0)
            throw new XtException(error);
    }

    static String wrapAndFreeString(Pointer p) {
        String result = p.getString(0);
        XtAudioFree(p);
        return result;
    }

    static native int XtErrorGetCause(long error);
    static native int XtErrorGetFault(long error);
    static native int XtErrorGetSystem(long error);
    static native String XtErrorGetText(long error);

    static native Pointer XtPrintErrorToString(long error);
    static native Pointer XtPrintMixToString(Mix mix);
    static native String XtPrintLevelToString(int level);
    static native String XtPrintCauseToString(int cause);
    static native String XtPrintSetupToString(int setup);
    static native String XtPrintSystemToString(int system);
    static native String XtPrintSampleToString(int sample);
    static native Pointer XtPrintFormatToString(Format format);
    static native Pointer XtPrintBufferToString(XtBuffer buffer);
    static native Pointer XtPrintLatencyToString(XtLatency latency);
    static native Pointer XtPrintChannelsToString(XtChannels channels);
    static native Pointer XtPrintCapabilitiesToString(int capabilities);
    static native Pointer XtPrintAttributesToString(XtAttributes attributes);

    static native void XtStreamDestroy(Pointer s);
    static native long XtStreamStop(Pointer s);
    static native long XtStreamStart(Pointer s);
    static native int XtStreamGetSystem(Pointer s);
    static native long XtStreamGetFrames(Pointer s, IntByReference frames);
    static native long XtStreamGetLatency(Pointer s, XtLatency latency);
    static native Pointer XtStreamGetFormat(Pointer s);
    static native boolean XtStreamIsInterleaved(Pointer s);

    static native int XtServiceGetSystem(Pointer s);
    static native String XtServiceGetName(Pointer s);
    static native int XtServiceGetCapabilities(Pointer s);
    static native long XtServiceGetDeviceCount(Pointer s, IntByReference count);
    static native long XtServiceOpenDevice(Pointer s, int index, PointerByReference device);
    static native long XtServiceOpenDefaultDevice(Pointer s, boolean output, PointerByReference device);
    static native long XtServiceAggregateStream(Pointer s, Pointer devices,
            Pointer channels, double[] bufferSizes, int count, Mix mix,
            boolean interleaved, Pointer master, StreamCallback streamCallback,
            XRunCallback xRunCallback, Pointer user, PointerByReference stream);

    static native boolean XtAudioIsWin32();
    static native void XtAudioTerminate();
    static native void XtAudioFree(Pointer p);
    static native int XtAudioGetVersionMajor();
    static native int XtAudioGetVersionMinor();
    static native int XtAudioGetServiceCount();
    static native Pointer XtAudioGetServiceByIndex(int index);
    static native Pointer XtAudioGetServiceBySetup(int setup);
    static native Pointer XtAudioGetServiceBySystem(int system);
    static native void XtAudioGetSampleAttributes(int sample, XtAttributes attributes);
    static native void XtAudioInit(String id, Pointer window, TraceCallback trace, XtFatalCallback fatal);

    static native void XtDeviceDestroy(Pointer d);
    static native long XtDeviceShowControlPanel(Pointer d);
    static native int XtDeviceGetSystem(Pointer d);
    static native long XtDeviceGetMix(Pointer d, PointerByReference mix);
    static native long XtDeviceGetName(Pointer d, PointerByReference name);
    static native long XtDeviceGetChannelCount(Pointer d, boolean output, IntByReference count);
    static native long XtDeviceGetBuffer(Pointer d, Format format, XtBuffer buffer);
    static native long XtDeviceSupportsFormat(Pointer d, Format format, IntByReference supports);
    static native long XtDeviceSupportsAccess(Pointer d, boolean interleaved, IntByReference supports);
    static native long XtDeviceGetChannelName(Pointer d, boolean output, int index, PointerByReference name);
    static native long XtDeviceOpenStream(Pointer d, Format format, boolean interleaved, double bufferSize, StreamCallback streamCallback, XRunCallback xRunCallback, Pointer user, PointerByReference stream);
}
