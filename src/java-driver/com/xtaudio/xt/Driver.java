package com.xtaudio.xt;

import com.sun.jna.Pointer;
import java.io.FileOutputStream;
import java.io.IOException;
import java.nio.ByteBuffer;
import java.time.LocalDateTime;
import java.time.format.DateTimeFormatter;

public class Driver {

    private static boolean AUTO = false;
    private static final boolean LIST = true;
    private static final int AUTO_MILLIS = 333;
    private static final boolean STREAM = true;
    private static final boolean LIST_FULL = true;
    private static final int AUTO_STREAM_MILLIS = -1;
    private static final double TONE_FREQUENCY = 660.0;

    private static final int[] CHANNELS = {1, 2, 6, 8};
    private static final XtSample[] SAMPLES = XtSample.values();
    private static final int[] RATES = {22050, 44100, 48000, 96000};

    private static class StreamContext {

        double phase;
        double start;
        long processed;
        ByteBuffer buffer;
        FileOutputStream recording;
    }

    private static boolean readLine(String s) {
        System.console().printf(s);
        System.console().printf("Enter q to quit, any key to continue...\n");
        return !System.console().readLine().equals("q");
    }

    private static void onFatal() {
        throw new RuntimeException("Driver fatal error.");
    }

    private static void onTrace(XtLevel level, String message) {
        if (level != XtLevel.INFO)
            System.console().printf("-- %s: %s\n", level, message);
    }

    public static void main(String[] args) {
        if (args.length > 0)
            AUTO = true;
        else if (!readLine(""))
            return;
        try (XtAudio audio = new XtAudio("xt-java-driver", Pointer.NULL, Driver::onTrace, Driver::onFatal)) {
            if (LIST && !list())
                return;
            if (STREAM && !stream())
                return;
        } catch (Exception e) {
            System.console().printf("Error: %s", e);
            e.printStackTrace();
        }
    }
    
    private static boolean list() {
        System.console().printf("Win32: %s\n", XtAudio.isWin32());
        System.console().printf("Version: %s\n", XtAudio.getVersion());
        System.console().printf("Service count: %s\n", XtAudio.getServiceCount());
        System.console().printf("%s setup: %s\n", XtSetup.CONSUMER_AUDIO.toString(), XtAudio.getServiceBySetup(XtSetup.CONSUMER_AUDIO).toString());
        System.console().printf("%s setup: %s\n", XtSetup.SYSTEM_AUDIO.toString(), XtAudio.getServiceBySetup(XtSetup.SYSTEM_AUDIO).toString());
        System.console().printf("%s setup: %s\n", XtSetup.PRO_AUDIO.toString(), XtAudio.getServiceBySetup(XtSetup.PRO_AUDIO).toString());
        for (int s = 0; s < XtAudio.getServiceCount(); s++)
            if (!listService(s))
                return false;
        return true;
    }

    private static boolean listService(int index) {
        XtService s = XtAudio.getServiceByIndex(index);
        System.console().printf("Service %s:\n", index);
        System.console().printf("\tName: %s\n", s.getName());
        System.console().printf("\tSystem: %s\n", s.getSystem());
        System.console().printf("\tCapabilities: %s\n", XtPrint.capabilitiesToString(s.getCapabilities()));
        System.console().printf("\tDevice count: %s\n", s.getDeviceCount());
        try (XtDevice defaultInput = s.openDefaultDevice(false)) {
            System.console().printf("\tDefault input: %s\n", defaultInput);
        }
        try (XtDevice defaultOutput = s.openDefaultDevice(true)) {
            System.console().printf("\tDefault output: %s\n", defaultOutput);
        }
        for (int d = 0; d < s.getDeviceCount(); d++)
            if (!listDevice(s, d))
                return false;
        return true;
    }

    private static boolean listDevice(XtService s, int index) {
        try (XtDevice d = s.openDevice(index)) {
            System.console().printf("\tDevice %s:\n", index);
            System.console().printf("\t\tName: %s\n", d.getName());
            System.console().printf("\t\tSystem: %s\n", d.getSystem());
            System.console().printf("\t\tCurrent mix: %s\n", d.getMix());
            if (LIST_FULL) {
                listChannels(d);
                listFormats(d);
                if (!listControlPanel(d))
                    return false;
            }
            return true;
        }
    }

    private static void listChannels(XtDevice d) {
        System.console().printf("\t\tInput channels: ");
        for (int c = 0; c < d.getChannelCount(false); c++)
            System.console().printf("%s, ", d.getChannelName(false, c));
        System.console().printf("\n");
        System.console().printf("\t\tOutput channels: ");
        for (int c = 0; c < d.getChannelCount(true); c++)
            System.console().printf("%s, ", d.getChannelName(true, c));
        System.console().printf("\n");
    }

    private static void listFormats(XtDevice d) {
        for (int r = 0; r < RATES.length; r++)
            for (int s = 0; s < SAMPLES.length; s++)
                for (int c = 0; c < CHANNELS.length; c++) {
                    XtFormat format = new XtFormat(new XtMix(RATES[r], SAMPLES[s]), CHANNELS[c], 0, 0, 0);
                    if (d.supportsFormat(format))
                        System.console().printf("\t\tSupports: %s (%s)\n", format, d.getBuffer(format));
                    format.inputs = 0;
                    format.outputs = CHANNELS[c];
                    if (d.supportsFormat(format))
                        System.console().printf("\t\tSupports: %s (%s)\n", format, d.getBuffer(format));
                    format.inputs = CHANNELS[c];
                    if (d.supportsFormat(format))
                        System.console().printf("\t\tSupports: %s (%s)\n", format, d.getBuffer(format));
                }
    }

    private static boolean listControlPanel(XtDevice d) {
        if (!AUTO && d.getSystem() == XtSystem.ASIO) {
            d.showControlPanel();
            return readLine("Please close ASIO control panel and press any key to continue...\n");
        }
        return true;
    }

    private static boolean stream() throws Exception {
        for (int s = 0; s < XtAudio.getServiceCount(); s++)
            if (!streamService(XtAudio.getServiceByIndex(s)))
                return false;
        return true;
    }

    private static boolean streamService(XtService s) throws Exception {
        try (XtDevice defaultInput = s.openDefaultDevice(false)) {
            if (defaultInput != null && !streamDevice(s, defaultInput))
                return false;
        }
        try (XtDevice defaultOutput = s.openDefaultDevice(true)) {
            if (defaultOutput != null && !streamDevice(s, defaultOutput))
                return false;
        }
        for (int d = 0; d < s.getDeviceCount(); d++)
            try (XtDevice device = s.openDevice(d)) {
                if (!streamDevice(s, device))
                    return false;
            }
        return true;
    }

    private static boolean streamDevice(XtService service, XtDevice d) throws Exception {
        for (int r = 0; r < RATES.length; r++)
            for (int s = 0; s < SAMPLES.length; s++) {
                int maxOutputs = 0;
                for (int c = 0; c < CHANNELS.length; c++) {
                    XtFormat format = new XtFormat(new XtMix(RATES[r], SAMPLES[s]), CHANNELS[c], 0, 0, 0);
                    if (d.supportsFormat(format) && !streamFormat(service, d, format))
                        return false;
                    format.inputs = 0;
                    format.outputs = CHANNELS[c];
                    if (d.supportsFormat(format)) {
                        maxOutputs = Math.max(maxOutputs, CHANNELS[c]);
                        if (!streamFormat(service, d, format))
                            return false;
                    }
                    format.inputs = CHANNELS[c];
                    if (d.supportsFormat(format) && !streamFormat(service, d, format))
                        return false;
                }
                if ((service.getCapabilities() & XtCapabilities.CHANNEL_MASK) != 0)
                    for (int c = 0; c < maxOutputs; c++) {
                        XtFormat format = new XtFormat(new XtMix(RATES[r], SAMPLES[s]), 0, 0, 1, 1L << c);
                        if (d.supportsFormat(format) && !streamFormat(service, d, format))
                            return false;
                    }
            }
        return true;
    }

    private static boolean streamFormat(XtService service, XtDevice d, XtFormat format) throws Exception {
        XtBuffer buffer = d.getBuffer(format);
        if (!streamBufferSize(service, d, format, buffer.min))
            return false;
        if (!streamBufferSize(service, d, format, buffer.max))
            return false;
        if (!streamBufferSize(service, d, format, buffer.current))
            return false;
        return true;
    }

    private static boolean streamBufferSize(XtService service, XtDevice d, XtFormat format, double bufferSize) throws Exception {

        StreamContext context = new StreamContext();
        context.phase = 0.0;
        context.start = -1.0;
        context.processed = 0;
        DateTimeFormatter formatter = DateTimeFormatter.ofPattern("yyyy.MM.dd.HH.mm.ss");
        String recordFileName = String.format("xt-recording-%s-%s-%s-%s-%s.raw",
                service, d, formatter.format(LocalDateTime.now()), format, bufferSize);
        System.console().printf("Streaming: %s: %s %s %s\n", service, d, format, bufferSize);
        try (XtStream stream = d.openStream(format, bufferSize, Driver::onStreamCallback, context)) {
            int frames = stream.getFrames();
            int sampleSize = XtAudio.getSampleAttributes(format.mix.sample).size;
            context.buffer = ByteBuffer.allocate(frames * format.inputs * sampleSize);
            System.console().printf("Latency: %s\n", stream.getLatency());
            System.console().printf("Buffer: %s (%s ms)\n", frames, frames * 1000.0 / format.mix.rate);
            if (format.outputs == 0)
                context.recording = new FileOutputStream(recordFileName);
            if (!AUTO && AUTO_STREAM_MILLIS == -1 && !readLine("Waiting for stream to start...\n"))
                return false;
            stream.start();
            if (!AUTO && AUTO_STREAM_MILLIS == -1 && !readLine("Waiting for stream to stop...\n"))
                return false;
            if (AUTO || AUTO_STREAM_MILLIS != -1)
                Thread.sleep(AUTO ? AUTO_MILLIS : AUTO_STREAM_MILLIS);
            stream.stop();
            return true;
        } finally {
            if (context.recording != null)
                context.recording.close();
        }
    }

    private static void onStreamCallback(XtStream stream, Object input, Object output, int frames,
            double time, long position, boolean timeValid, long error, Object user) {

        double value;
        StreamContext ctx = (StreamContext) user;
        XtFormat format = stream.getFormat();
        int outputs = format.outputs;
        int sampleSize = XtAudio.getSampleAttributes(format.mix.sample).size;
        int bufferSizeBytes = frames * format.inputs * sampleSize;
        assert (format.inputs == 0 || format.outputs == 0 || format.inputs == format.outputs);

        if (error != 0) {
            System.out.println(String.format("Stream error: %s\n", XtPrint.errorToString(error)));
            return;
        }

        if (frames == 0)
            return;

        if (format.outputs == 0)
            writeRecording(ctx, format, input, frames, bufferSizeBytes);
        else if (format.inputs != 0)
            System.arraycopy(input, 0, output, 0, frames * format.inputs);
        else
            for (int f = 0; f < frames; f++) {
                ctx.phase += TONE_FREQUENCY / format.mix.rate;
                if (ctx.phase > 1.0)
                    ctx.phase = -1.0;
                value = Math.sin(ctx.phase * Math.PI) * 0.95;
                for (int c = 0; c < format.outputs; c++)
                    outputSine(output, f * outputs + c, format.mix.sample, value);
            }

        if (ctx.start < 0.0)
            ctx.start = time;
        ctx.processed += frames;
        if (ctx.processed > format.mix.rate) {
            ctx.processed = 0;
            System.out.println(String.format("Time: %s, position: %s, valid: %s, latency: %s\n",
                    time - ctx.start, position, timeValid, stream.getLatency()));
        }
    }

    private static void outputSine(Object dest, int pos, XtSample sample, double value) {
        int ivalue;
        switch (sample) {
            case FLOAT32:
                ((float[]) dest)[pos] = (float) value;
                break;
            case INT32:
                ((int[]) dest)[pos] = (int) (value * Integer.MAX_VALUE);
                break;
            case UINT8:
                ((byte[]) dest)[pos] = (byte) ((value + 1.0) / 2.0 * Byte.MAX_VALUE);
                break;
            case INT16:
                ((short[]) dest)[pos] = (short) (value * Short.MAX_VALUE);
                break;
            case INT24:
                ivalue = (int) (value * Integer.MAX_VALUE);
                ((byte[]) dest)[0] = (byte) ((ivalue & 0x0000FF00) >> 8);
                ((byte[]) dest)[1] = (byte) ((ivalue & 0x00FF0000) >> 16);
                ((byte[]) dest)[2] = (byte) ((ivalue & 0xFF000000) >> 24);
                break;
            default:
                assert (false);
        }
    }

    private static void writeRecording(StreamContext ctx, XtFormat format, Object input, int frames, int bufferSizeBytes) {

        ctx.buffer.clear();        
        switch (format.mix.sample) {
            case UINT8:
                ctx.buffer.put((byte[]) input, 0, frames * format.inputs);
                break;
            case INT16:
                ctx.buffer.asShortBuffer().put((short[]) input, 0, frames * format.inputs);
                break;
            case INT24:
                ctx.buffer.put((byte[]) input, 0, frames * format.inputs * 3);
                break;
            case INT32:
                ctx.buffer.asIntBuffer().put((int[]) input, 0, frames * format.inputs);
                break;
            case FLOAT32:
                ctx.buffer.asFloatBuffer().put((float[]) input, 0, frames * format.inputs);
                break;
        }
        try {
            ctx.recording.write(ctx.buffer.array(), 0, bufferSizeBytes);
        } catch (IOException ex) {
            throw new RuntimeException(ex);
        }
    }
}
