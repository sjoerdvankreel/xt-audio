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

    static void readLine() {
        System.out.println("Press any key to continue...");
        System.console().readLine();
    }

    static int getBufferSize(XtStream stream, int frames) {
        XtFormat format = stream.getFormat();
        int sampleSize = XtAudio.getSampleAttributes(format.mix.sample).size;
        return frames * format.channels.inputs * sampleSize;
    }

    static void xRun(int index, Pointer user) {
        // Don't do this.
        System.out.println("XRun on device " + index + ".");
    }

    static void captureInterleavedManaged(Pointer stream, XtBuffer buffer, Pointer user) throws Exception {
        XtAdapter adapter = XtAdapter.get(stream);
        adapter.lockBuffer(buffer);
        // Don't do this.
        if(buffer.frames > 0)
            ((Context)adapter.getUser()).out.write((byte[])adapter.getInput(), 0, getBufferSize(adapter.getStream(), buffer.frames));
        adapter.unlockBuffer(buffer);
    }

    static void captureInterleavedNative(Pointer stream, XtBuffer buffer, Pointer user) throws Exception {
        XtAdapter adapter = XtAdapter.get(stream);
        if(buffer.frames > 0) {
            Context ctx = (Context)adapter.getUser();
            ((Pointer)buffer.input).read(0, ctx.intermediate, 0, getBufferSize(adapter.getStream(), buffer.frames));
            // Don't do this.
            ctx.out.write(ctx.intermediate, 0, getBufferSize(adapter.getStream(), buffer.frames));
        }
    }

    static void captureNonInterleavedManaged(Pointer stream, XtBuffer buffer, Pointer user) throws Exception {
        XtAdapter adapter = XtAdapter.get(stream);
        adapter.lockBuffer(buffer);
        if(buffer.frames > 0) {
            Context ctx = (Context)adapter.getUser();
            XtFormat format = adapter.getStream().getFormat();
            int channels = format.channels.inputs;
            int sampleSize = XtAudio.getSampleAttributes(format.mix.sample).size;
            for(int f = 0; f < buffer.frames; f++)
                for(int c = 0; c < channels; c++)
                    // Don't do this.
                    ctx.out.write(((byte[][])adapter.getInput())[c], f * sampleSize, sampleSize);
        }
        adapter.unlockBuffer(buffer);
    }

    static void captureNonInterleavedNative(Pointer stream, XtBuffer buffer, Pointer user) throws Exception {
        XtAdapter adapter = XtAdapter.get(stream);
        if(buffer.frames > 0) {
            Context ctx = (Context)adapter.getUser();
            XtFormat format = adapter.getStream().getFormat();
            int channels = format.channels.inputs;
            int sampleSize = XtAudio.getSampleAttributes(format.mix.sample).size;
            for(int f = 0; f < buffer.frames; f++)
                for(int c = 0; c < channels; c++) {
                    buffer.input.getPointer(c * Native.POINTER_SIZE).read(f * sampleSize, ctx.intermediate, 0, sampleSize);
                    // Don't do this.
                    ctx.out.write(ctx.intermediate, 0, sampleSize);
                }
        }
    }

    public static void main(String[] args) throws Exception {

        try(XtAudio audio = new XtAudio(null, null, null)) {

            var system = XtAudio.setupToSystem(XtSetup.CONSUMER_AUDIO);
            XtService service = XtAudio.getService(system);
            if(service == null)
                return;

            XtFormat format = new XtFormat(new XtMix(44100, XtSample.INT24), new XtChannels(2, 0, 0, 0));
            try(XtDevice device = service.openDefaultDevice(false)) {
                if(device == null || !device.supportsFormat(format))
                    return;

                Context context = new Context();
                XtBufferSize size = device.getBufferSize(format);

                try(FileOutputStream recording = new FileOutputStream("xt-audio-interleaved.raw");
                    XtStream stream = device.openStream(format, true, size.current,
                            CaptureAdvanced::captureInterleavedManaged, CaptureAdvanced::xRun);
                    XtAdapter adapter = XtAdapter.register(stream, true, context)) {
                    context.out = recording;
                    context.intermediate = new byte[getBufferSize(stream, stream.getFrames())];
                    stream.start();
                    System.out.println("Capturing interleaved...");
                    readLine();
                    stream.stop();
                }

                try(FileOutputStream recording = new FileOutputStream("xt-audio-interleaved-raw.raw");
                    XtStream stream = device.openStream(format, true, size.current,
                            CaptureAdvanced::captureInterleavedNative, CaptureAdvanced::xRun);
                    XtAdapter adapter = XtAdapter.register(stream, true, context)) {
                    context.out = recording;
                    context.intermediate = new byte[getBufferSize(stream, stream.getFrames())];
                    stream.start();
                    System.out.println("Capturing interleaved, raw buffers...");
                    readLine();
                    stream.stop();
                }

                try(FileOutputStream recording = new FileOutputStream("xt-audio-non-interleaved.raw");
                    XtStream stream = device.openStream(format, false, size.current,
                            CaptureAdvanced::captureNonInterleavedManaged, CaptureAdvanced::xRun);
                    XtAdapter adapter=XtAdapter.register(stream, false, context)) {
                    context.out = recording;
                    context.intermediate = new byte[getBufferSize(stream, stream.getFrames())];
                    stream.start();
                    System.out.println("Capturing non-interleaved...");
                    readLine();
                    stream.stop();
                }

                try(FileOutputStream recording = new FileOutputStream("xt-audio-non-interleaved-raw.raw");
                    XtStream stream = device.openStream(format, false, size.current,
                            CaptureAdvanced::captureNonInterleavedNative, CaptureAdvanced::xRun);
                    XtAdapter adapter = XtAdapter.register(stream, false, context)) {
                    context.out = recording;
                    context.intermediate = new byte[getBufferSize(stream, stream.getFrames())];
                    stream.start();
                    System.out.println("Capturing non-interleaved, raw buffers...");
                    readLine();
                    stream.stop();
                }
            }
        }
    }
}