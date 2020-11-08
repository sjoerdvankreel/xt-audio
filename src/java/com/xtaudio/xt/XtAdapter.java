package com.xtaudio.xt;

import java.lang.reflect.*;
import java.nio.*;
import java.util.*;

import com.sun.jna.*;
import com.xtaudio.xt.NativeTypes.*;

public final class XtAdapter implements XtCloseable {

    static final Map<Pointer, XtAdapter> _map = new HashMap<Pointer, XtAdapter>();
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

    static void fromNative(Pointer source, Object dest, XtSample sample, int count)
    {
        switch(sample)
        {

        }
    }

    private final int _frames;
    private final int _inputs;
    private final int _outputs;
    private final Object _user;
    private final Object _input;
    private final Object _output;
    private final byte[] _scratch;
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
        int chans = Math.max(_inputs, _outputs);
        _scratch = new byte[chans * _frames * _attrs.size];
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
        int bytes = _inputs * buffer.frames * _attrs.size;
        Marshal.Copy(buffer.input, _scratch, 0, bytes);
        Buffer.BlockCopy(_scratch, 0, _input, 0, bytes);
    }

    void unlockInterleaved(XtBuffer buffer) {
        int bytes = _outputs * buffer.frames * _attrs.size;
        Buffer.BlockCopy(_output, 0, _scratch, 0, bytes);
        Marshal.Copy(_scratch, 0, buffer.output, bytes);
    }

    void lockChannel(XtBuffer buffer, int channel) {
        int bytes = buffer.frames * _attrs.size;
        Marshal.Copy(((IntPtr *)buffer.input)[channel], _scratch, 0, bytes);
        Buffer.BlockCopy(_scratch, 0, (Array)_input.GetValue(channel), 0, bytes);
    }

    void unlockChannel(XtBuffer buffer, int channel) {
        int bytes = buffer.frames * _attrs.size;
        Buffer.BlockCopy((Array)_output.GetValue(channel), 0, _scratch, 0, bytes);
        Marshal.Copy(_scratch, 0, ((IntPtr *)buffer.output)[channel], bytes);
    }
}