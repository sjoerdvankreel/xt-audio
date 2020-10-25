package com.xtaudio.xt;

import com.sun.jna.Callback;
import com.sun.jna.Pointer;
import com.sun.jna.Structure;
import com.sun.jna.TypeMapper;
import java.util.Arrays;
import java.util.List;
import com.xtaudio.xt.XtNative.XtTypeMapper;

public final class NativeTypes {

    public static enum XtLevel {
        INFO,
        ERROR,
        FATAL;
    }

    public static enum XtSetup {
        PRO_AUDIO,
        SYSTEM_AUDIO,
        CONSUMER_AUDIO;
    }

    public static enum XtSystem {
        ALSA,
        ASIO,
        JACK,
        PULSE,
        DSOUND,
        WASAPI;
    }

    public static enum XtSample {
        UINT8,
        INT16,
        INT24,
        INT32,
        FLOAT32;
    }

    public static enum XtCause {
        FORMAT,
        SERVICE,
        GENERIC,
        UNKNOWN,
        ENDPOINT;
    }

    public static final class XtCapabilities {

        public static final int NONE = 0x0;
        public static final int TIME = 0x1;
        public static final int LATENCY = 0x2;
        public static final int FULL_DUPLEX = 0x4;
        public static final int CHANNEL_MASK = 0x8;
        public static final int XRUN_DETECTION = 0x10;

        public static String toString(int capabilities) {
            return String.join(", ", XtNative.XtAudioPrintCapabilitiesToString(capabilities));
        }
    }
    
    public static final class XtVersion extends Structure implements Structure.ByValue
    {
        public int major;
        public int minor;

        @Override
        protected List getFieldOrder() {
            return Arrays.asList("major", "minor");
        }
    }

    public static final class XtLatency extends Structure {

        public double input;
        public double output;

        @Override
        protected List getFieldOrder() {
            return Arrays.asList("input", "output");
        }
    }

    public static final class XtFormat {

        public XtMix mix = new XtMix();
        public XtChannels channels = new XtChannels();

        public XtFormat() {
        }

        public XtFormat(XtMix mix, XtChannels channels) {
            this.mix = mix;
            this.channels = channels;
        }
    }

    public static final class XtBuffer extends Structure {

        public double min;
        public double max;
        public double current;

        @Override
        protected List getFieldOrder() {
            return Arrays.asList("min", "max", "current");
        }
    }

    public static final class XtAttributes extends Structure implements Structure.ByValue {

        public int size;
        public boolean isFloat;
        public boolean isSigned;

        @Override
        protected List getFieldOrder() {
            return Arrays.asList("size", "isFloat", "isSigned");
        }
    }
    
    public static final class XtErrorInfo extends Structure implements Structure.ByValue {        
        
        public static final TypeMapper TYPE_MAPPER = new XtTypeMapper();
        
        public XtSystem system;
        public XtCause cause;
        public String text;
        public int fault;
        
        @Override
        protected List getFieldOrder() {
            return Arrays.asList("system", "cause", "text", "fault");
        }
    }

    public static final class XtMix extends Structure {

        public static final TypeMapper TYPE_MAPPER = new XtTypeMapper();

        public int rate;
        public XtSample sample;

        public XtMix() {
        }

        public XtMix(int rate, XtSample sample) {
            this.rate = rate;
            this.sample = sample;
        }

        @Override
        protected List getFieldOrder() {
            return Arrays.asList("rate", "sample");
        }
    }

    public static class XtChannels extends Structure {

        public int inputs;
        public long inMask;
        public int outputs;
        public long outMask;

        public XtChannels() {
        }

        public XtChannels(int inputs, long inMask, int outputs, long outMask) {
            this.inputs = inputs;
            this.inMask = inMask;
            this.outputs = outputs;
            this.outMask = outMask;
        }

        void doUseMemory(Pointer m, int offset) {
            super.useMemory(m, offset);
        }

        @Override
        protected List getFieldOrder() {
            return Arrays.asList("inputs", "inMask", "outputs", "outMask");
        }
    }

    public static interface XtFatalCallback extends Callback {

        void callback();
    }

    public static interface XtXRunCallback {

        void callback(int index, Object user) throws Exception;
    }

    public static interface XtTraceCallback {

        void callback(XtLevel level, String message);
    }

    public static interface XtStreamCallback {

        void callback(XtStream stream, Object input, Object output, int frames, double time, long position, boolean timeValid, long error, Object user) throws Exception;
    }
}
