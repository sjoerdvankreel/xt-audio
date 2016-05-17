package com.xtaudio.xt;

import com.sun.jna.Native;
import com.sun.jna.NativeLibrary;
import com.sun.jna.Pointer;
import java.io.FileOutputStream;
import java.io.IOException;
import java.lang.reflect.Array;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
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

    private static native Pointer memcpy(Pointer dest, Pointer src, Pointer count);

    private static class StreamContext {

        double phase;
        double start;
        long processed;
        ByteBuffer buffer;
        byte[] intermediate;
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
            if (XtAudio.isWin32())
                Native.register(NativeLibrary.getInstance("msvcrt"));
            else
                Native.register(NativeLibrary.getInstance("libc"));
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
            System.console().printf("\t\tInterleaved access: %s\n", d.supportsAccess(true));
            System.console().printf("\t\tNon-interleaved access: %s\n", d.supportsAccess(false));
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

    private static boolean streamBufferSize(XtService service, XtDevice d,
            XtFormat format, double bufferSize) throws Exception {

        if (!streamRawMode(service, d, format, bufferSize, true))
            return false;
        if (!streamRawMode(service, d, format, bufferSize, false))
            return false;
        return true;
    }

    private static boolean streamRawMode(XtService service, XtDevice d, XtFormat format,
            double bufferSize, boolean interleaved) throws Exception {

        if (!streamAccessMode(service, d, format, bufferSize, interleaved, false))
            return false;
        if (!streamAccessMode(service, d, format, bufferSize, interleaved, true))
            return false;
        return true;
    }

    private static boolean streamAccessMode(XtService service, XtDevice d, XtFormat format,
            double bufferSize, boolean interleaved, boolean raw) throws Exception {

        StreamContext context = new StreamContext();
        context.phase = 0.0;
        context.start = -1.0;
        context.processed = 0;
        DateTimeFormatter formatter = DateTimeFormatter.ofPattern("yyyy.MM.dd.HH.mm.ss");
        String recordFileName = String.format("xt-recording-%s-%s-%s-%s-%s-%s-%s.raw",
                service, d, formatter.format(LocalDateTime.now()), format, bufferSize, interleaved, raw);
        String fmtString = "Streaming: %s: %s, format %s, buffer %s, interleaved %s, raw %s\n";
        System.console().printf(fmtString, service, d, format, bufferSize, interleaved, raw);
        try (XtStream stream = d.openStream(format, interleaved, raw, bufferSize, Driver::onStreamCallback, context)) {
            int frames = stream.getFrames();
            int sampleSize = XtAudio.getSampleAttributes(format.mix.sample).size;
            context.intermediate = new byte[frames * format.inputs * sampleSize];
            context.buffer = ByteBuffer.allocate(frames * format.inputs * sampleSize).order(ByteOrder.LITTLE_ENDIAN);
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
        boolean raw = stream.isRaw();
        boolean interleaved = stream.isInterleaved();
        StreamContext ctx = (StreamContext) user;
        XtFormat format = stream.getFormat();
        int outputs = format.outputs;
        int sampleElements = format.mix.sample == XtSample.INT24 ? 3 : 1;
        int sampleSize = XtAudio.getSampleAttributes(format.mix.sample).size;
        int bufferBytes = frames * format.inputs * sampleSize;
        assert (format.inputs == 0 || format.outputs == 0 || format.inputs == format.outputs);

        if (error != 0) {
            System.out.println(String.format("Stream error: %s\n", XtPrint.errorToString(error)));
            return;
        }

        if (frames == 0)
            return;

        if (format.outputs == 0)
            if (!raw)
                if (interleaved)
                    writeRecording(ctx, format, input, 0, frames * format.inputs * sampleElements, bufferBytes);
                else
                    for (int f = 0; f < frames; f++)
                        for (int c = 0; c < format.inputs; c++)
                            writeRecording(ctx, format, Array.get(input, c), f * sampleElements, sampleElements, sampleSize);
            else if (interleaved)
                writeRecordingRaw(ctx, (Pointer) input, 0, bufferBytes);
            else
                for (int f = 0; f < frames; f++)
                    for (int c = 0; c < format.inputs; c++)
                        writeRecordingRaw(ctx, ((Pointer) input).getPointer(c * Pointer.SIZE), f * sampleSize, sampleSize);
        else if (format.inputs != 0)
            if (!raw)
                if (interleaved)
                    System.arraycopy(input, 0, output, 0, frames * format.inputs * sampleElements);
                else
                    for (int c = 0; c < format.inputs; c++)
                        System.arraycopy(Array.get(input, c), 0, Array.get(output, c), 0, frames * sampleElements);
            else if (interleaved)
                memcpy((Pointer) output, (Pointer) input, new Pointer(frames * format.inputs * sampleSize));
            else
                for (int c = 0; c < format.inputs; c++)
                    memcpy(
                            ((Pointer) output).getPointer(c * Pointer.SIZE),
                            ((Pointer) input).getPointer(c * Pointer.SIZE),
                            new Pointer(frames * sampleSize));
        else
            for (int f = 0; f < frames; f++) {
                ctx.phase += TONE_FREQUENCY / format.mix.rate;
                if (ctx.phase > 1.0)
                    ctx.phase = -1.0;
                value = Math.sin(ctx.phase * Math.PI) * 0.95;
                if (!raw)
                    if (interleaved)
                        for (int c = 0; c < format.outputs; c++)
                            outputSine(output, (f * outputs + c) * sampleElements, format.mix.sample, value);
                    else
                        for (int c = 0; c < format.outputs; c++)
                            outputSine(Array.get(output, 0), f * sampleElements, format.mix.sample, value);
                else if (interleaved)
                    for (int c = 0; c < format.outputs; c++)
                        outputSineRaw((Pointer) output, format.mix.sample, f * outputs + c, value);
                else
                    for (int c = 0; c < format.outputs; c++)
                        outputSineRaw(((Pointer) output).getPointer(c * Pointer.SIZE), format.mix.sample, f, value);
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
                ((byte[]) dest)[pos + 0] = (byte) ((ivalue & 0x0000FF00) >> 8);
                ((byte[]) dest)[pos + 1] = (byte) ((ivalue & 0x00FF0000) >> 16);
                ((byte[]) dest)[pos + 2] = (byte) ((ivalue & 0xFF000000) >> 24);
                break;
            default:
                assert (false);
        }
    }

    private static void outputSineRaw(Pointer dest, XtSample sample, int frame, double value) {
        int ivalue;
        switch (sample) {
            case FLOAT32:
                dest.setFloat(frame * 4, (float) value);
                break;
            case INT32:
                dest.setInt(frame * 4, (int) (value * Integer.MAX_VALUE));
                break;
            case UINT8:
                dest.setByte(frame, (byte) ((value + 1.0) / 2.0 * Byte.MAX_VALUE));
                break;
            case INT16:
                dest.setShort(frame * 2, (short) (value * Short.MAX_VALUE));
                break;
            case INT24:
                ivalue = (int) (value * Integer.MAX_VALUE);
                dest.setByte(frame * 3, (byte) ((ivalue & 0x0000FF00) >> 8));
                dest.setByte(frame * 3 + 1, (byte) ((ivalue & 0x00FF0000) >> 16));
                dest.setByte(frame * 3 + 2, (byte) ((ivalue & 0xFF000000) >> 24));
                break;
            default:
                assert (false);
        }
    }

    private static void writeRecording(StreamContext ctx, XtFormat format, Object input, int offset, int length, int bytes) {

        ctx.buffer.clear();
        switch (format.mix.sample) {
            case UINT8:
                ctx.buffer.put((byte[]) input, offset, length);
                break;
            case INT16:
                ctx.buffer.asShortBuffer().put((short[]) input, offset, length);
                break;
            case INT24:
                ctx.buffer.put((byte[]) input, offset, length);
                break;
            case INT32:
                ctx.buffer.asIntBuffer().put((int[]) input, offset, length);
                break;
            case FLOAT32:
                ctx.buffer.asFloatBuffer().put((float[]) input, offset, length);
                break;
        }
        try {
            ctx.recording.write(ctx.buffer.array(), 0, bytes);
        } catch (IOException ex) {
            throw new RuntimeException(ex);
        }
    }

    private static void writeRecordingRaw(StreamContext ctx, Pointer input, int offsetBytes, int lengthBytes) {
        ctx.buffer.clear();
        input.read(offsetBytes, ctx.intermediate, 0, lengthBytes);
        ctx.buffer.put(ctx.intermediate, 0, lengthBytes);
        try {
            ctx.recording.write(ctx.buffer.array(), 0, lengthBytes);
        } catch (IOException ex) {
            throw new RuntimeException(ex);
        }
    }
}
