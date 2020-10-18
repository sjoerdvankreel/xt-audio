package com.xtaudio.xt.sample;

import com.sun.jna.Native;
import com.sun.jna.Pointer;
import com.xtaudio.xt.XtAudio;
import com.xtaudio.xt.XtBuffer;
import com.xtaudio.xt.XtChannels;
import com.xtaudio.xt.XtDevice;
import com.xtaudio.xt.XtFormat;
import com.xtaudio.xt.XtMix;
import com.xtaudio.xt.XtSample;
import com.xtaudio.xt.XtService;
import com.xtaudio.xt.XtSetup;
import com.xtaudio.xt.XtStream;
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

    static void xRun(int index, Object user) {
        // Don't do this.
        System.out.println("XRun on device " + index + ".");
    }

    static void captureInterleaved(XtStream stream, Object input, Object output, int frames,
            double time, long position, boolean timeValid, long error, Object user) throws Exception {

        // Don't do this.
        if (frames > 0)
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
            int channels = format.channels.inputs;
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
            int channels = format.channels.inputs;
            int sampleSize = XtAudio.getSampleAttributes(format.mix.sample).size;
            for (int f = 0; f < frames; f++)
                for (int c = 0; c < channels; c++) {
                    ((Pointer) input).getPointer(c * Native.POINTER_SIZE).read(f * sampleSize, ctx.intermediate, 0, sampleSize);
                    // Don't do this.
                    ctx.out.write(ctx.intermediate, 0, sampleSize);
                }
        }
    }

    public static void main(String[] args) throws Exception {

        try (XtAudio audio = new XtAudio(null, null, null, null)) {

            XtService service = XtAudio.getServiceBySetup(XtSetup.CONSUMER_AUDIO);
            if (service == null)
                return;

            XtFormat format = new XtFormat(new XtMix(44100, XtSample.INT24), new XtChannels(2, 0, 0, 0));
            try (XtDevice device = service.openDefaultDevice(false)) {
                if (device == null || !device.supportsFormat(format))
                    return;

                Context context = new Context();
                XtBuffer buffer = device.getBuffer(format);

                try (FileOutputStream recording = new FileOutputStream("xt-audio-interleaved.raw");
                        XtStream stream = device.openStream(format, true, false, buffer.current,
                                CaptureAdvanced::captureInterleaved, CaptureAdvanced::xRun, context)) {
                    context.out = recording;
                    context.intermediate = new byte[getBufferSize(stream, stream.getFrames())];
                    stream.start();
                    System.out.println("Capturing interleaved...");
                    readLine();
                    stream.stop();
                }

                try (FileOutputStream recording = new FileOutputStream("xt-audio-interleaved-raw.raw");
                        XtStream stream = device.openStream(format, true, true, buffer.current,
                                CaptureAdvanced::captureInterleavedRaw, CaptureAdvanced::xRun, context)) {
                    context.out = recording;
                    context.intermediate = new byte[getBufferSize(stream, stream.getFrames())];
                    stream.start();
                    System.out.println("Capturing interleaved, raw buffers...");
                    readLine();
                    stream.stop();
                }

                try (FileOutputStream recording = new FileOutputStream("xt-audio-non-interleaved.raw");
                        XtStream stream = device.openStream(format, false, false, buffer.current,
                                CaptureAdvanced::captureNonInterleaved, CaptureAdvanced::xRun, context)) {
                    context.out = recording;
                    context.intermediate = new byte[getBufferSize(stream, stream.getFrames())];
                    stream.start();
                    System.out.println("Capturing non-interleaved...");
                    readLine();
                    stream.stop();
                }

                try (FileOutputStream recording = new FileOutputStream("xt-audio-non-interleaved-raw.raw");
                        XtStream stream = device.openStream(format, false, true, buffer.current,
                                CaptureAdvanced::captureNonInterleavedRaw, CaptureAdvanced::xRun, context)) {
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
