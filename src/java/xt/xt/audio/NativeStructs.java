package xt.audio;

import com.sun.jna.Pointer;
import com.sun.jna.Structure;
import xt.audio.NativeCallbacks.OnBuffer;
import xt.audio.NativeCallbacks.OnRunning;
import xt.audio.NativeCallbacks.OnXRun;
import xt.audio.Structs.XtChannels;
import xt.audio.Structs.XtFormat;
import xt.audio.Structs.XtMix;
import java.util.Arrays;
import java.util.List;

interface NativeStructs {

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
        public OnRunning onRunning;
        public StreamParams() {}
        @Override protected List getFieldOrder() { return Arrays.asList("interleaved", "onBuffer", "onXRun", "onRunning"); }
    }
}