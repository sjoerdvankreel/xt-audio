package com.xtaudio.xt.sample;

import com.sun.jna.Native;
import com.xtaudio.xt.*;
import static com.xtaudio.xt.NativeTypes.*;

public class RenderAdvanced {

    static float _phase = 0.0f;
    static final float FREQUENCY = 440.0f;
    static final XtMix MIX = new XtMix(44100, XtSample.FLOAT32);

    static float nextSample() {
        _phase += FREQUENCY / MIX.rate;
        if(_phase >= 1.0) _phase = -1.0f;
        return (float)Math.sin(2.0 * _phase * Math.PI);
    }

    static void xRun(int index, Object user) {
        System.out.println("XRun on device " + index + ".");
    }

    static void renderInterleavedSafe(XtStream stream, XtBuffer buffer, Object user) throws Exception {
        XtSafeBuffer safe = XtSafeBuffer.get(stream);
        int channels = stream.getFormat().channels.outputs;
        safe.lock(buffer);
        float[] output = (float[])safe.getOutput();
        for(int f = 0; f < buffer.frames; f++) {
            float sample = nextSample();
            for(int c = 0; c < channels; c++) output[f * channels + c] = sample;
        }
        safe.unlock(buffer);
    }

    static void renderInterleavedNative(XtStream stream, XtBuffer buffer, Object user) throws Exception {
        int channels = stream.getFormat().channels.outputs;
        int size = XtAudio.getSampleAttributes(MIX.sample).size;
        for(int f = 0; f < buffer.frames; f++) {
            float sample = nextSample();
            for(int c = 0; c < channels; c++)
                buffer.output.setFloat((f * channels + c) * size, sample);
        }
    }

    static void renderNonInterleavedSafe(XtStream stream, XtBuffer buffer, Object user) throws Exception {
        XtSafeBuffer safe = XtSafeBuffer.get(stream);
        int channels = stream.getFormat().channels.outputs;
        safe.lock(buffer);
        float[][] output = (float[][])safe.getOutput();
        for(int f = 0; f < buffer.frames; f++) {
            float sample = nextSample();
            for(int c = 0; c < channels; c++) output[c][f] = sample;
        }
        safe.unlock(buffer);
    }

    static void renderNonInterleavedNative(XtStream stream, XtBuffer buffer, Object user) throws Exception {
        int channels = stream.getFormat().channels.outputs;
        int size = XtAudio.getSampleAttributes(MIX.sample).size;
        for(int f = 0; f < buffer.frames; f++) {
            float sample = nextSample();
            for(int c = 0; c < channels; c++)
                buffer.output.getPointer(c * Native.POINTER_SIZE).setFloat(f * size, sample);
        }
    }

    public static void main(String[] args) throws Exception {
        try(XtAudio audio = new XtAudio(null, null, null)) {
            XtSystem system = XtAudio.setupToSystem(XtSetup.CONSUMER_AUDIO);
            XtService service = XtAudio.getService(system);
            if(service == null) return;
            XtFormat format = new XtFormat(MIX, new XtChannels(0, 0, 2, 0));
            try(XtDevice device = service.openDefaultDevice(true)) {
                if(device == null || !device.supportsFormat(format)) return;
                XtBufferSize size = device.getBufferSize(format);

                System.out.println("Render interleaved, safe buffers...");
                try(XtStream stream = device.openStream(format, true, size.current,
                        RenderAdvanced::renderInterleavedSafe, RenderAdvanced::xRun, null);
                    XtSafeBuffer safe = XtSafeBuffer.register(stream, true)) {
                    stream.start();
                    Thread.sleep(2000);
                    stream.stop();
                }

                System.out.println("Render interleaved, native buffers...");
                try(XtStream stream = device.openStream(format, true, size.current,
                        RenderAdvanced::renderInterleavedNative, RenderAdvanced::xRun, null)) {
                    stream.start();
                    Thread.sleep(2000);
                    stream.stop();
                }

                System.out.println("Render non-interleaved, safe buffers...");
                try(XtStream stream = device.openStream(format, false, size.current,
                        RenderAdvanced::renderNonInterleavedSafe, RenderAdvanced::xRun, null);
                    XtSafeBuffer safe = XtSafeBuffer.register(stream, false)) {
                    stream.start();
                    Thread.sleep(2000);
                    stream.stop();
                }

                System.out.println("Render non-interleaved, native buffers...");
                try(XtStream stream = device.openStream(format, false, size.current,
                        RenderAdvanced::renderNonInterleavedNative, RenderAdvanced::xRun, null)) {
                    stream.start();
                    Thread.sleep(2000);
                    stream.stop();
                }

                System.out.println("Render interleaved, safe buffers (channel 0)...");
                XtFormat sendTo0 = new XtFormat(MIX, new XtChannels(0, 0, 1, 1L << 0));
                try(XtStream stream = device.openStream(sendTo0, true, size.current,
                        RenderAdvanced::renderInterleavedSafe, RenderAdvanced::xRun, null);
                    XtSafeBuffer safe = XtSafeBuffer.register(stream, true)) {
                    stream.start();
                    Thread.sleep(2000);
                    stream.stop();
                }

                System.out.println("Render interleaved, native buffers (channel 1)...");
                XtFormat sendTo1 = new XtFormat(MIX, new XtChannels(0, 0, 1, 1L << 1));
                try(XtStream stream = device.openStream(sendTo1, true, size.current,
                        RenderAdvanced::renderInterleavedNative, RenderAdvanced::xRun, null)) {
                    stream.start();
                    Thread.sleep(2000);
                    stream.stop();
                }
            }
        }
    }
}