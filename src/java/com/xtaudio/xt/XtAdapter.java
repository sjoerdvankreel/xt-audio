package com.xtaudio.xt;

import java.lang.reflect.*;
import java.util.*;

import com.sun.jna.*;
import com.xtaudio.xt.NativeTypes.*;

public final class XtAdapter implements XtCloseable {

    static final Map<Pointer, XtAdapter> _map = new HashMap<>();
    static final Map<XtSample, Class<?>> _types = Map.of(
            XtSample.UINT8, byte.class,
            XtSample.INT16, short.class,
            XtSample.INT24, byte.class,
            XtSample.INT32, int.class,
            XtSample.FLOAT32, float.class
    );

    public static XtAdapter register(XtStream stream, boolean interleaved, Object user) {
        var result = new XtAdapter(stream, interleaved, user);
        _map.put(stream.handle(), result);
        return result;
    }

    private final int _frames;
    private final int _inputs;
    private final int _outputs;
    private final Object _user;
    private final Object _input;
    private final Object _output;
    private final XtStream _stream;
    private final XtFormat _format;
    private final XtAttributes _attrs;
    private final boolean _interleaved;

    public Object getUser() { return _user; }
    public Object getInput() { return _input; }
    public Object getOutput() { return _output; }
    public XtStream getStream() { return _stream; }
    public void close() { _map.remove(_stream.handle()); }
    public static XtAdapter get(Pointer stream) { return _map.get(stream); }

    XtAdapter(XtStream stream, boolean interleaved, Object user) {
        _user = user;
        _stream = stream;
        _interleaved = interleaved;
        _format = stream.getFormat();
        _frames = stream.getFrames();
        _inputs = _format.channels.inputs;
        _outputs = _format.channels.outputs;
        _attrs = XtAudio.getSampleAttributes(_format.mix.sample);
        _input = createBuffer(_inputs);
        _output = createBuffer(_outputs);
    }

    Object createBuffer(int channels) {
        var type = _types.get(_format.mix.sample);
        int elems = _frames * _attrs.count;
        if(_interleaved) return Array.newInstance(type, channels * elems);
        var result = Array.newInstance(type.arrayType(), channels);
        for(int i = 0; i < channels; i++) Array.set(result, i, Array.newInstance(type, elems));
        return result;
    }

    public void lockBuffer(XtBuffer buffer) {
        if(buffer.input == Pointer.NULL) return;
        if(_interleaved) lockInterleaved(buffer);
        else for(int i = 0; i < _inputs; i++) lockChannel(buffer, i);
    }

    public void unlockBuffer(XtBuffer buffer) {
        if(buffer.output == Pointer.NULL) return;
        if(_interleaved) unlockInterleaved(buffer);
        else for(int i = 0; i < _outputs; i++) unlockChannel(buffer, i);
    }

    void lockInterleaved(XtBuffer buffer) {
        int elems = _inputs * buffer.frames * _attrs.count;
        fromNative(buffer.input, _input, elems);
    }

    void unlockInterleaved(XtBuffer buffer) {
        int elems = _outputs * buffer.frames * _attrs.count;
        toNative(_output, buffer.output, elems);
    }

    void lockChannel(XtBuffer buffer, int channel) {
        int elems = buffer.frames * _attrs.count;
        var channelBuffer = buffer.input.getPointer(channel * Native.POINTER_SIZE);
        fromNative(channelBuffer, Array.get(_input, channel), elems);
    }

    void unlockChannel(XtBuffer buffer, int channel) {
        int elems = buffer.frames * _attrs.count;
        var channelBuffer = buffer.output.getPointer(channel * Native.POINTER_SIZE);
        toNative(Array.get(_output, channel), channelBuffer, elems);
    }

    void toNative(Object source, Pointer dest, int count) {
        switch(_format.mix.sample) {
        case UINT8: dest.write(0, (byte[])source, 0, count);
        case INT16: dest.write(0, (short[])source, 0, count);
        case INT24: dest.write(0, (byte[])source, 0, count);
        case INT32: dest.write(0, (int[])source, 0, count);
        case FLOAT32: dest.write(0, (float[])source, 0, count);
        default: throw new IllegalArgumentException();
        }
    }

    void fromNative(Pointer source, Object dest, int count) {
        switch(_format.mix.sample) {
        case UINT8: source.read(0, (byte[])dest, 0, count);
        case INT16: source.read(0, (short[])dest, 0, count);
        case INT24: source.read(0, (byte[])dest, 0, count);
        case INT32: source.read(0, (int[])dest, 0, count);
        case FLOAT32: source.read(0, (float[])dest, 0, count);
        default: throw new IllegalArgumentException();
        }
    }
}