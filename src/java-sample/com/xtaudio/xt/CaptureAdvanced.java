package com.xtaudio.xt;

import com.sun.jna.Pointer;
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
        return frames * format.inputs * sampleSize;
    }

    static void captureInterleaved(XtStream stream, Object input, Object output, int frames,
            double time, long position, boolean timeValid, long error, Object user) throws Exception {

        if (frames > 0)
            // Don't do this.
            ((Context) user).out.write((byte[]) input, 0, getBufferSize(stream, frames));
    }

    static void captureInterleavedRaw(XtStream stream, Object input, Object output, int frames,
            double time, long position, boolean timeValid, long error, Object user) throws Exception {

        if (frames > 0) {
            Context ctx = (Context) user;
            ((Pointer) input).read(0, ctx.intermediate, 0, getBufferSize(stream, frames));
            // Don't do this.
            ctx.out.write(ctx.intermediate, 0, getBufferSize(stream, frames));
        }
    }

    static void captureNonInterleaved(XtStream stream, Object input, Object output, int frames,
            double time, long position, boolean timeValid, long error, Object user) throws Exception {

        if (frames > 0) {
            Context ctx = (Context) user;
            XtFormat format = stream.getFormat();
            int channels = format.inputs;
            int sampleSize = XtAudio.getSampleAttributes(format.mix.sample).size;
            for (int f = 0; f < frames; f++)
                for (int c = 0; c < channels; c++)
                    // Don't do this.
                    ctx.out.write(((byte[][]) input)[c], f * sampleSize, sampleSize);
        }
    }

    static void captureNonInterleavedRaw(XtStream stream, Object input, Object output, int frames,
            double time, long position, boolean timeValid, long error, Object user) throws Exception {

        if (frames > 0) {
            Context ctx = (Context) user;
            XtFormat format = stream.getFormat();
            int channels = format.inputs;
            int sampleSize = XtAudio.getSampleAttributes(format.mix.sample).size;
            for (int f = 0; f < frames; f++)
                for (int c = 0; c < channels; c++) {
                    ((Pointer) input).getPointer(c * Pointer.SIZE).read(f * sampleSize, ctx.intermediate, 0, sampleSize);
                    // Don't do this.
                    ctx.out.write(ctx.intermediate, 0, sampleSize);
                }
        }
    }

    public static void main(String[] args) throws Exception {

        try (XtAudio audio = new XtAudio(null, null, null, null)) {

            XtService service = XtAudio.getServiceBySetup(XtSetup.CONSUMER_AUDIO);
            XtFormat format = new XtFormat(new XtMix(44100, XtSample.INT24), 2, 0, 0, 0);
            try (XtDevice device = service.openDefaultDevice(false)) {

                if (device == null) {
                    System.out.println("No default device found.");
                    return;
                }

                if (!device.supportsFormat(format)) {
                    System.out.println("Format not supported.");
                    return;
                }

                Context context = new Context();
                XtBuffer buffer = device.getBuffer(format);

                try (FileOutputStream recording = new FileOutputStream("xt-audio-interleaved.raw");
                        XtStream stream = device.openStream(format, true, false, buffer.current,
                                CaptureAdvanced::captureInterleaved, context)) {
                    context.out = recording;
                    context.intermediate = new byte[getBufferSize(stream, stream.getFrames())];
                    stream.start();
                    System.out.println("Capturing interleaved...");
                    readLine();
                    stream.stop();
                }

                try (FileOutputStream recording = new FileOutputStream("xt-audio-interleaved-raw.raw");
                        XtStream stream = device.openStream(format, true, true, buffer.current,
                                CaptureAdvanced::captureInterleavedRaw, context)) {
                    context.out = recording;
                    context.intermediate = new byte[getBufferSize(stream, stream.getFrames())];
                    stream.start();
                    System.out.println("Capturing interleaved, raw buffers...");
                    readLine();
                    stream.stop();
                }

                try (FileOutputStream recording = new FileOutputStream("xt-audio-non-interleaved.raw");
                        XtStream stream = device.openStream(format, false, false, buffer.current,
                                CaptureAdvanced::captureNonInterleaved, context)) {
                    context.out = recording;
                    context.intermediate = new byte[getBufferSize(stream, stream.getFrames())];
                    stream.start();
                    System.out.println("Capturing non-interleaved...");
                    readLine();
                    stream.stop();
                }

                try (FileOutputStream recording = new FileOutputStream("xt-audio-non-interleaved-raw.raw");
                        XtStream stream = device.openStream(format, false, true, buffer.current,
                                CaptureAdvanced::captureNonInterleavedRaw, context)) {
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
