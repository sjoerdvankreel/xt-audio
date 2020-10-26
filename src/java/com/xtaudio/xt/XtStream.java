package com.xtaudio.xt;

import com.sun.jna.Native;
import com.sun.jna.Pointer;
import com.sun.jna.ptr.IntByReference;
import com.xtaudio.xt.NativeTypes.XtFormat;
import com.xtaudio.xt.NativeTypes.XtLatency;
import com.xtaudio.xt.NativeTypes.XtLevel;
import com.xtaudio.xt.NativeTypes.XtSample;
import com.xtaudio.xt.NativeTypes.XtStreamCallback;
import com.xtaudio.xt.NativeTypes.XtXRunCallback;
import java.lang.reflect.Array;

public final class XtStream implements XtCloseable {

    private static Object createNonInterleavedBuffer(XtSample sample, int channels, int frames) {
        switch (sample) {
            case UINT8:
                return Array.newInstance(byte.class, channels, frames);
            case INT16:
                return Array.newInstance(short.class, channels, frames);
            case INT24:
                return Array.newInstance(byte.class, channels, frames * 3);
            case INT32:
                return Array.newInstance(int.class, channels, frames);
            case FLOAT32:
                return Array.newInstance(float.class, channels, frames);
            default:
                throw new IllegalArgumentException();
        }
    }

    private static Object createInterleavedBuffer(XtSample sample, int channels, int frames) {
        switch (sample) {
            case UINT8:
                return Array.newInstance(byte.class, channels * frames);
            case INT16:
                return Array.newInstance(short.class, channels * frames);
            case INT24:
                return Array.newInstance(byte.class, channels * frames * 3);
            case INT32:
                return Array.newInstance(int.class, channels * frames);
            case FLOAT32:
                return Array.newInstance(float.class, channels * frames);
            default:
                throw new IllegalArgumentException();
        }
    }

    private static void copyInterleavedBufferFromNative(XtSample sample, Pointer raw, Object managed, int channels, int frames) {
        switch (sample) {
            case UINT8:
                raw.read(0, (byte[]) managed, 0, channels * frames);
                break;
            case INT16:
                raw.read(0, (short[]) managed, 0, channels * frames);
                break;
            case INT24:
                raw.read(0, (byte[]) managed, 0, channels * frames * 3);
                break;
            case INT32:
                raw.read(0, (int[]) managed, 0, channels * frames);
                break;
            case FLOAT32:
                raw.read(0, (float[]) managed, 0, channels * frames);
                break;
            default:
                throw new IllegalArgumentException();
        }
    }

    private static void copyInterleavedBufferToNative(XtSample sample, Object managed, Pointer raw, int channels, int frames) {
        switch (sample) {
            case UINT8:
                raw.write(0, (byte[]) managed, 0, channels * frames);
                break;
            case INT16:
                raw.write(0, (short[]) managed, 0, channels * frames);
                break;
            case INT24:
                raw.write(0, (byte[]) managed, 0, channels * frames * 3);
                break;
            case INT32:
                raw.write(0, (int[]) managed, 0, channels * frames);
                break;
            case FLOAT32:
                raw.write(0, (float[]) managed, 0, channels * frames);
                break;
            default:
                throw new IllegalArgumentException();
        }
    }

    private static void copyNonInterleavedBufferFromNative(XtSample sample, Pointer raw, Object managed, int channels, int frames) {
        switch (sample) {
            case UINT8:
                for (int i = 0; i < channels; i++)
                    raw.getPointer(i * Native.POINTER_SIZE).read(0, ((byte[][]) managed)[i], 0, frames);
                break;
            case INT16:
                for (int i = 0; i < channels; i++)
                    raw.getPointer(i * Native.POINTER_SIZE).read(0, ((short[][]) managed)[i], 0, frames);
                break;
            case INT24:
                for (int i = 0; i < channels; i++)
                    raw.getPointer(i * Native.POINTER_SIZE).read(0, ((byte[][]) managed)[i], 0, frames * 3);
                break;
            case INT32:
                for (int i = 0; i < channels; i++)
                    raw.getPointer(i * Native.POINTER_SIZE).read(0, ((int[][]) managed)[i], 0, frames);
                break;
            case FLOAT32:
                for (int i = 0; i < channels; i++)
                    raw.getPointer(i * Native.POINTER_SIZE).read(0, ((float[][]) managed)[i], 0, frames);
                break;
            default:
                throw new IllegalArgumentException();
        }
    }

    private static void copyNonInterleavedBufferToNative(XtSample sample, Object managed, Pointer raw, int channels, int frames) {
        switch (sample) {
            case UINT8:
                for (int i = 0; i < channels; i++)
                    raw.getPointer(i * Native.POINTER_SIZE).write(0, ((byte[][]) managed)[i], 0, frames);
                break;
            case INT16:
                for (int i = 0; i < channels; i++)
                    raw.getPointer(i * Native.POINTER_SIZE).write(0, ((short[][]) managed)[i], 0, frames);
                break;
            case INT24:
                for (int i = 0; i < channels; i++)
                    raw.getPointer(i * Native.POINTER_SIZE).write(0, ((byte[][]) managed)[i], 0, frames * 3);
                break;
            case INT32:
                for (int i = 0; i < channels; i++)
                    raw.getPointer(i * Native.POINTER_SIZE).write(0, ((int[][]) managed)[i], 0, frames);
                break;
            case FLOAT32:
                for (int i = 0; i < channels; i++)
                    raw.getPointer(i * Native.POINTER_SIZE).write(0, ((float[][]) managed)[i], 0, frames);
                break;
            default:
                throw new IllegalArgumentException();
        }
    }

    private final Object user;
    private final boolean raw;
    private final XtXRunCallback userXRunCallback;
    private final XtStreamCallback userStreamCallback;

    private Pointer s;
    private Object inputInterleaved;
    private Object outputInterleaved;
    private Object inputNonInterleaved;
    private Object outputNonInterleaved;
    final XtNative.XRunCallback nativeXRunCallback;
    final XtNative.StreamCallback nativeStreamCallback;

    XtStream(boolean raw, XtStreamCallback userStreamCallback, XtXRunCallback userXRunCallback, Object user) {
        this.raw = raw;
        this.user = user;
        this.userXRunCallback = userXRunCallback;
        this.userStreamCallback = userStreamCallback;
        this.nativeXRunCallback = this::xRunCallback;
        this.nativeStreamCallback = this::streamCallback;
    }

    public boolean isRaw() {
        return raw;
    }

    public void stop() {
        XtNative.handleError(XtNative.XtStreamStop(s));
    }

    public void start() {
        XtNative.handleError(XtNative.XtStreamStart(s));
    }

    public boolean isInterleaved() {
        return XtNative.XtStreamIsInterleaved(s);
    }

    public XtFormat getFormat() {
        return new XtNative.Format(XtNative.XtStreamGetFormat(s)).fromNative();
    }

    @Override
    public void close() {
        if (s != null)
            XtNative.XtStreamDestroy(s);
        s = null;
    }

    public int getFrames() {
        IntByReference frames = new IntByReference();
        XtNative.handleError(XtNative.XtStreamGetFrames(s, frames));
        return frames.getValue();
    }

    public XtLatency getLatency() {
        XtLatency latency = new XtLatency();
        XtNative.handleError(XtNative.XtStreamGetLatency(s, latency));
        return latency;
    }

    void init(Pointer s) {
        this.s = s;
        if (!isRaw()) {
            int frames = getFrames();
            XtFormat format = getFormat();
            if (isInterleaved()) {
                inputInterleaved = createInterleavedBuffer(format.mix.sample, format.channels.inputs, frames);
                outputInterleaved = createInterleavedBuffer(format.mix.sample, format.channels.outputs, frames);
            } else {
                inputNonInterleaved = createNonInterleavedBuffer(format.mix.sample, format.channels.inputs, frames);
                outputNonInterleaved = createNonInterleavedBuffer(format.mix.sample, format.channels.outputs, frames);
            }
        }
    }

    void xRunCallback(int index, Pointer user) {
        try {
            userXRunCallback.callback(index, this.user);
        } catch (Throwable t) {
            if (XtAudio.trace != null)
                XtAudio.trace.callback(XtLevel.FATAL, String.format("Exception caught in xrun callback: %s.", t));
            System.out.println(t);
            t.printStackTrace();
            Runtime.getRuntime().halt(1);
        }
    }

    void streamCallback(Pointer stream, Pointer input, Pointer output, int frames,
            double time, long position, boolean timeValid, long error, Pointer u) {

        XtFormat format = getFormat();
        boolean interleaved = isInterleaved();
        Object inData = raw ? input : input == null ? null : interleaved ? inputInterleaved : inputNonInterleaved;
        Object outData = raw ? output : output == null ? null : interleaved ? outputInterleaved : outputNonInterleaved;

        if (!raw && inData != null)
            if (interleaved)
                copyInterleavedBufferFromNative(format.mix.sample, input, inData, format.channels.inputs, frames);
            else
                copyNonInterleavedBufferFromNative(format.mix.sample, input, inData, format.channels.inputs, frames);

        try {
            userStreamCallback.callback(this, inData, outData, frames, time, position, timeValid, error, user);
        } catch (Throwable t) {
            if (XtAudio.trace != null)
                XtAudio.trace.callback(XtLevel.FATAL, String.format("Exception caught in stream callback: %s.", t));
            System.out.println(t);
            t.printStackTrace();
            Runtime.getRuntime().halt(1);
        }

        if (!raw && outData != null)
            if (interleaved)
                copyInterleavedBufferToNative(format.mix.sample, outData, output, format.channels.outputs, frames);
            else
                copyNonInterleavedBufferToNative(format.mix.sample, outData, output, format.channels.outputs, frames);
    }
}