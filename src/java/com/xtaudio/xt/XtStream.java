package com.xtaudio.xt;

import com.sun.jna.Pointer;
import com.sun.jna.ptr.IntByReference;

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
public final class XtStream implements XtCloseable {

    private Pointer s;
    private Object input;
    private Object output;
    private final Object user;
    XtNative.StreamCallback nativeCallback;
    private final XtStreamCallback userCallback;

    XtStream(XtStreamCallback userCallback, Object user) {
        this.user = user;
        this.userCallback = userCallback;
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

    public XtSystem getSystem() {
        return XtSystem.class.getEnumConstants()[XtNative.XtStreamGetSystem(s) - 1];
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
        int frames = getFrames();
        XtFormat format = getFormat();
        switch (format.mix.sample) {
            case UINT8:
                input = new byte[format.inputs * frames];
                output = new byte[format.outputs * frames];
                break;
            case INT16:
                input = new short[format.inputs * frames];
                output = new short[format.outputs * frames];
                break;
            case INT24:
                input = new byte[format.inputs * 3 * frames];
                output = new byte[format.outputs * 3 * frames];
                break;
            case INT32:
                input = new int[format.inputs * frames];
                output = new int[format.outputs * frames];
                break;
            case FLOAT32:
                input = new float[format.inputs * frames];
                output = new float[format.outputs * frames];
                break;
            default:
                throw new IllegalArgumentException();
        }
    }

    void callback(Pointer stream, Pointer input, Pointer output, int frames,
            double time, long position, boolean timeValid, long error, Pointer u) {

        XtFormat format = getFormat();
        Object inData = input == null ? null : this.input;
        Object outData = output == null ? null : this.output;
        
        if (inData != null)
            switch (format.mix.sample) {
                case UINT8:
                    input.read(0, (byte[]) inData, 0, frames * format.inputs);
                    break;
                case INT16:
                    input.read(0, (short[]) inData, 0, frames * format.inputs);
                    break;
                case INT24:
                    input.read(0, (byte[]) inData, 0, frames * format.inputs * 3);
                    break;
                case INT32:
                    input.read(0, (int[]) inData, 0, frames * format.inputs);
                    break;
                case FLOAT32:
                    input.read(0, (float[]) inData, 0, frames * format.inputs);
                    break;
            }
        try {
            userCallback.callback(this, inData, outData, frames, time, position, timeValid, error, user);
        } catch (Throwable t) {
            if (XtAudio.trace != null)
                XtAudio.trace.callback(XtLevel.FATAL, String.format("Exception caught in stream callback: %s.", t));
            t.printStackTrace();
            Runtime.getRuntime().halt(1);
        }
        if (outData != null)
            switch (format.mix.sample) {
                case UINT8:
                    output.write(0, (byte[]) outData, 0, frames * format.outputs);
                    break;
                case INT16:
                    output.write(0, (short[]) outData, 0, frames * format.outputs);
                    break;
                case INT24:
                    output.write(0, (byte[]) outData, 0, frames * format.outputs * 3);
                    break;
                case INT32:
                    output.write(0, (int[]) outData, 0, frames * format.outputs);
                    break;
                case FLOAT32:
                    output.write(0, (float[]) outData, 0, frames * format.outputs);
                    break;
            }
    }
}
