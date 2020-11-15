package com.xtaudio.xt;

import com.sun.jna.Callback;
import com.sun.jna.Pointer;
import com.sun.jna.Structure;
import com.sun.jna.TypeMapper;
import com.xtaudio.xt.XtNative.XtTypeMapper;

import java.util.Arrays;
import java.util.List;

public final class NativeTypes {

    public enum XtSample { UINT8, INT16, INT24, INT32, FLOAT32 }
    public enum XtSetup { PRO_AUDIO, SYSTEM_AUDIO, CONSUMER_AUDIO }
    public enum XtCause { FORMAT, SERVICE, GENERIC, UNKNOWN, ENDPOINT }
    public enum XtSystem { ALSA, ASIO, JACK, WASAPI, PULSE_AUDIO, DIRECT_SOUND }

    public enum XtCapabilities {
        NONE(0x0), TIME(0x1), LATENCY(0x2), FULL_DUPLEX(0x4), CHANNEL_MASK(0x8), XRUN_DETECTION(0x10);
        final int _flag;
        private XtCapabilities(int flag) { _flag = flag; }
    }

    public static class DeviceStreamParams extends Structure {
        public StreamParams stream;
        public XtFormat format;
        public double bufferSize;
        public DeviceStreamParams() {}
        @Override protected List getFieldOrder() { return Arrays.asList("stream", "format", "bufferSize"); }
    }

    public static class AggregateDeviceParams extends Structure {
        public Pointer device;
        public XtChannels channels;
        public double bufferSize;
        public AggregateDeviceParams() {}
        public static class ByValue extends AggregateDeviceParams implements Structure.ByValue {}
        @Override protected List getFieldOrder() { return Arrays.asList("device", "channels", "bufferSize"); }
    }

    public static class AggregateStreamParams extends Structure {
        public StreamParams stream;
        public Pointer devices;
        public int count;
        public XtMix mix;
        public Pointer master;
        public AggregateStreamParams() {}
        @Override protected List getFieldOrder() { return Arrays.asList("stream", "devices", "count", "mix", "master"); }
    }

    public static class StreamParams extends Structure {
        public boolean interleaved;
        public OnBuffer onBuffer;
        public OnXRun onXRun;
        public StreamParams() {}
        @Override protected List getFieldOrder() { return Arrays.asList("interleaved", "onBuffer", "onXRun"); }
    }

    public static class XtMix extends Structure {
        public XtMix() { }
        public int rate;
        public XtSample sample;
        public static final TypeMapper TYPE_MAPPER = new XtTypeMapper();
        public XtMix(int rate, XtSample sample) { this.rate = rate; this.sample = sample; }
        @Override protected List getFieldOrder() { return Arrays.asList("rate", "sample"); }
    }

    public static class XtVersion extends Structure {
        public int major;
        public int minor;
        public static class ByValue extends XtVersion implements Structure.ByValue {}
        @Override protected List getFieldOrder() { return Arrays.asList("major", "minor"); }
    }

    public static class XtLatency extends Structure {
        public double input;
        public double output;
        @Override protected List getFieldOrder() { return Arrays.asList("input", "output"); }
    }

    public static class XtDeviceStreamParams {
        public XtStreamParams stream;
        public XtFormat format;
        public double bufferSize;
        public XtDeviceStreamParams() {}
        public XtDeviceStreamParams(XtStreamParams stream, XtFormat format, double bufferSize) {
            this.stream = stream; this.format = format; this.bufferSize = bufferSize;
        }
    }

    public static class XtAggregateDeviceParams {
        public XtDevice device;
        public XtChannels channels;
        public double bufferSize;
        public XtAggregateDeviceParams() {}
        public XtAggregateDeviceParams(XtDevice device, XtChannels channels, double bufferSize) {
            this.device = device; this.channels = channels; this.bufferSize = bufferSize;
        }
    }

    public static class XtBufferSize extends Structure {
        public double min;
        public double max;
        public double current;
        @Override protected List getFieldOrder() { return Arrays.asList("min", "max", "current"); }
    }

    public static class XtFormat extends Structure {
        public XtFormat() { }
        public XtMix mix = new XtMix();
        public XtChannels channels = new XtChannels();
        @Override protected List getFieldOrder() { return Arrays.asList("mix", "channels"); }
        public XtFormat(XtMix mix, XtChannels channels) { this.mix = mix; this.channels = channels; }
    }

    public static class XtBuffer extends Structure {
        public Pointer input;
        public Pointer output;
        public double time;
        public long position;
        public long error;
        public int frames;
        public boolean timeValid;
        public static class ByValue extends XtBuffer implements Structure.ByValue {}
        @Override protected List getFieldOrder() {
            return Arrays.asList("input", "output", "time", "position", "error", "frames", "timeValid");
        }
    }

    public static class XtErrorInfo extends Structure {
        public XtSystem system;
        public XtCause cause;
        public String text;
        public int fault;
        public static final TypeMapper TYPE_MAPPER = new XtTypeMapper();
        public static class ByValue extends XtErrorInfo implements Structure.ByValue {}
        @Override public String toString() { return XtNative.XtPrintErrorInfoToString(this); }
        @Override protected List getFieldOrder() { return Arrays.asList("system", "cause", "text", "fault"); }
    }

    public static class XtAttributes extends Structure {
        public int size;
        public int count;
        public boolean isFloat;
        public boolean isSigned;
        public static class ByValue extends XtAttributes implements Structure.ByValue {}
        @Override protected List getFieldOrder() { return Arrays.asList("size", "count", "isFloat", "isSigned"); }
    }

    public static class XtStreamParams {
        public boolean interleaved;
        public XtOnBuffer onBuffer;
        public XtOnXRun onXRun;
        public XtStreamParams() {}
        public XtStreamParams(boolean interleaved, XtOnBuffer onBuffer, XtOnXRun onXRun) {
            this.interleaved = interleaved; this.onBuffer = onBuffer; this.onXRun = onXRun;
        }
    }

    public static class XtChannels extends Structure {
        public int inputs;
        public long inMask;
        public int outputs;
        public long outMask;
        public XtChannels() { }
        public static class ByValue extends XtChannels implements Structure.ByValue {}
        @Override protected List getFieldOrder() { return Arrays.asList("inputs", "inMask", "outputs", "outMask"); }
        public XtChannels(int inputs, long inMask, int outputs, long outMask) {
            this.inputs = inputs; this.inMask = inMask; this.outputs = outputs; this.outMask = outMask;
        }
    }

    public static class XtAggregateStreamParams {
        public XtStreamParams stream;
        public XtAggregateDeviceParams[] devices;
        public int count;
        public XtMix mix;
        public XtDevice master;
        public XtAggregateStreamParams() {}
        public XtAggregateStreamParams(XtStreamParams stream, XtAggregateDeviceParams[] devices, int count, XtMix mix, XtDevice master) {
            this.stream = stream; this.devices = devices; this.count = count; this.mix = mix; this.master = master;
        }
    }

    interface OnXRun extends Callback {
        void callback(int index, Pointer user) throws Exception;
    }

    public interface XtOnXRun {
        void callback(int index, Object user) throws Exception;
    }

    public interface XtOnError extends Callback {
        void callback(String location, String message) throws Exception;
    }

    interface OnBuffer extends Callback {
        void callback(Pointer stream, Pointer buffer, Pointer user) throws Exception;
    }

    public interface XtOnBuffer {
        void callback(XtStream stream, XtBuffer buffer, Object user) throws Exception;
    }
}