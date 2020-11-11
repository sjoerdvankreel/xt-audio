package com.xtaudio.xt.sample;

import com.sun.jna.Native;
import com.sun.jna.Pointer;
import com.xtaudio.xt.*;
import static com.xtaudio.xt.NativeTypes.*;

import java.io.FileOutputStream;

public class CaptureAdvanced {

    static class Context {
        byte[] intermediate;
        FileOutputStream out;
    }

    static final XtMix MIX = new XtMix(44100, XtSample.INT24);
    static final XtChannels CHANNELS = new XtChannels(2, 0, 0, 0);
    static final XtFormat FORMAT = new XtFormat(MIX, CHANNELS);

    static void xRun(int index, Object user) {
        System.out.println("XRun on device " + index + ".");
    }

    static int getBufferSize(int channels, int frames) {
        int size = XtAudio.getSampleAttributes(MIX.sample).size;
        return channels * frames * size;
    }

    static void captureInterleavedSafe(XtStream stream, XtBuffer buffer, Object user) throws Exception {
        var out = (FileOutputStream)user;
        XtSafeBuffer safe = XtSafeBuffer.get(stream);
        int bytes = getBufferSize(CHANNELS.inputs, buffer.frames);
        safe.lock(buffer);
        out.write((byte[])safe.getInput(), 0, bytes);
        safe.unlock(buffer);
    }

    static void captureInterleavedNative(XtStream stream, XtBuffer buffer, Object user) throws Exception {
        var ctx = (Context)user;
        int bytes = getBufferSize(CHANNELS.inputs, buffer.frames);
        buffer.input.read(0, ctx.intermediate, 0, bytes);
        ctx.out.write(ctx.intermediate, 0, bytes);
    }

    static void captureNonInterleavedSafe(XtStream stream, XtBuffer buffer, Object user) throws Exception {
        var out = (FileOutputStream)user;
        XtSafeBuffer safe = XtSafeBuffer.get(stream);
        int size = XtAudio.getSampleAttributes(MIX.sample).size;
        safe.lock(buffer);
        for(int f = 0; f < buffer.frames; f++)
            for(int c = 0; c < CHANNELS.inputs; c++)
                out.write(((byte[][])safe.getInput())[c], f * size, size);
        safe.unlock(buffer);
    }

    static void captureNonInterleavedNative(XtStream stream, XtBuffer buffer, Object user) throws Exception {
        var ctx = (Context)user;
        int size = XtAudio.getSampleAttributes(MIX.sample).size;
        for(int f = 0; f < buffer.frames; f++)
            for(int c = 0; c < CHANNELS.inputs; c++) {
                Pointer channel = buffer.input.getPointer(c * Native.POINTER_SIZE);
                channel.read(f * size, ctx.intermediate, 0, size);
                ctx.out.write(ctx.intermediate, 0, size);
            }
    }

    public static void main(String[] args) throws Exception {
        try(XtAudio audio = new XtAudio(null, null, null)) {
            XtSystem system = XtAudio.setupToSystem(XtSetup.CONSUMER_AUDIO);
            XtService service = XtAudio.getService(system);
            if(service == null) return;
            try(XtDevice device = service.openDefaultDevice(false)) {
                if(device == null || !device.supportsFormat(FORMAT)) return;
                XtBufferSize size = device.getBufferSize(FORMAT);

                System.out.println("Capture interleaved, safe buffers...");
                try(FileOutputStream recording = new FileOutputStream("xt-audio-interleaved-safe.raw");
                    XtStream stream = device.openStream(FORMAT, true, size.current,
                            CaptureAdvanced::captureInterleavedSafe, CaptureAdvanced::xRun, recording);
                    XtSafeBuffer safe = XtSafeBuffer.register(stream, true)) {
                    stream.start();
                    Thread.sleep(2000);
                    stream.stop();
                }

                System.out.println("Capture interleaved, native buffers...");
                Context context = new Context();
                try(FileOutputStream recording = new FileOutputStream("xt-audio-interleaved-native.raw");
                    XtStream stream = device.openStream(FORMAT, true, size.current,
                            CaptureAdvanced::captureInterleavedNative, CaptureAdvanced::xRun, context)) {
                    context.out = recording;
                    context.intermediate = new byte[getBufferSize(CHANNELS.inputs, stream.getFrames())];
                    stream.start();
                    Thread.sleep(2000);
                    stream.stop();
                }

                System.out.println("Capture non-interleaved, safe buffers...");
                try(FileOutputStream recording = new FileOutputStream("xt-audio-non-interleaved-safe.raw");
                    XtStream stream = device.openStream(FORMAT, false, size.current,
                            CaptureAdvanced::captureNonInterleavedSafe, CaptureAdvanced::xRun, recording);
                    XtSafeBuffer safe = XtSafeBuffer.register(stream, false)) {
                    stream.start();
                    Thread.sleep(2000);
                    stream.stop();
                }

                System.out.println("Capture non-interleaved, native buffers...");
                context = new Context();
                try(FileOutputStream recording = new FileOutputStream("xt-audio-non-interleaved-native.raw");
                    XtStream stream = device.openStream(FORMAT, false, size.current,
                            CaptureAdvanced::captureNonInterleavedNative, CaptureAdvanced::xRun, context)) {
                    context.out = recording;
                    context.intermediate = new byte[getBufferSize(1, stream.getFrames())];
                    stream.start();
                    Thread.sleep(2000);
                    stream.stop();
                }
            }
        }
    }
}