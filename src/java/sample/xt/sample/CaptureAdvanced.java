package xt.sample;

import com.sun.jna.Native;
import com.sun.jna.Pointer;

import xt.audio.Enums.XtSample;
import xt.audio.Enums.XtSetup;
import xt.audio.Enums.XtSystem;
import xt.audio.Structs.XtBuffer;
import xt.audio.Structs.XtBufferSize;
import xt.audio.Structs.XtChannels;
import xt.audio.Structs.XtDeviceStreamParams;
import xt.audio.Structs.XtFormat;
import xt.audio.Structs.XtMix;
import xt.audio.Structs.XtStreamParams;
import xt.audio.XtAudio;
import xt.audio.XtDevice;
import xt.audio.XtPlatform;
import xt.audio.XtSafeBuffer;
import xt.audio.XtService;
import xt.audio.XtStream;
import java.io.FileOutputStream;

public class CaptureAdvanced {

    static class Context {
        byte[] intermediate;
        FileOutputStream out;
    }

    static final XtMix MIX = new XtMix(44100, XtSample.INT24);
    static final XtChannels CHANNELS = new XtChannels(2, 0, 0, 0);
    static final XtFormat FORMAT = new XtFormat(MIX, CHANNELS);

    // Normally don't do I/O in the callback.
    static void onXRun(XtStream stream, int index, Object user) {
        System.out.println("XRun on device " + index + ".");
    }

    static void onRunning(XtStream stream, boolean running, long error, Object user) {
        String evt = running? "Started": "Stopped";
        System.out.println("Stream event: " + evt + ", new state: " + stream.isRunning() + ".");
        if(error != 0) System.out.println(XtAudio.getErrorInfo(error).toString());
    }

    static int getBufferSize(int channels, int frames) {
        int size = XtAudio.getSampleAttributes(MIX.sample).size;
        return channels * frames * size;
    }

    static void runStream(XtStream stream) throws Exception {
        stream.start();
        Thread.sleep(2000);
        stream.stop();
    }

    // Normally don't do I/O in the callback.
    static int onInterleavedSafeBuffer(XtStream stream, XtBuffer buffer, Object user) throws Exception {
        var out = (FileOutputStream)user;
        XtSafeBuffer safe = XtSafeBuffer.get(stream);
        int bytes = getBufferSize(CHANNELS.inputs, buffer.frames);
        safe.lock(buffer);
        out.write((byte[])safe.getInput(), 0, bytes);
        safe.unlock(buffer);
        return 0;
    }

    // Normally don't do I/O in the callback.
    static int onInterleavedNativeBuffer(XtStream stream, XtBuffer buffer, Object user) throws Exception {
        var ctx = (Context)user;
        int bytes = getBufferSize(CHANNELS.inputs, buffer.frames);
        buffer.input.read(0, ctx.intermediate, 0, bytes);
        ctx.out.write(ctx.intermediate, 0, bytes);
        return 0;
    }

    // Normally don't do I/O in the callback.
    static int onNonInterleavedSafeBuffer(XtStream stream, XtBuffer buffer, Object user) throws Exception {
        var out = (FileOutputStream)user;
        XtSafeBuffer safe = XtSafeBuffer.get(stream);
        int size = XtAudio.getSampleAttributes(MIX.sample).size;
        safe.lock(buffer);
        for(int f = 0; f < buffer.frames; f++)
            for(int c = 0; c < CHANNELS.inputs; c++)
                out.write(((byte[][])safe.getInput())[c], f * size, size);
        safe.unlock(buffer);
        return 0;
    }

    // Normally don't do I/O in the callback.
    static int onNonInterleavedNativeBuffer(XtStream stream, XtBuffer buffer, Object user) throws Exception {
        var ctx = (Context)user;
        int size = XtAudio.getSampleAttributes(MIX.sample).size;
        for(int f = 0; f < buffer.frames; f++)
            for(int c = 0; c < CHANNELS.inputs; c++) {
                Pointer channel = buffer.input.getPointer(c * Native.POINTER_SIZE);
                channel.read(f * size, ctx.intermediate, 0, size);
                ctx.out.write(ctx.intermediate, 0, size);
            }
        return 0;
    }

    public static void main() throws Exception {
        XtStreamParams streamParams;
        XtDeviceStreamParams deviceParams;

        try(XtPlatform platform = XtAudio.init(null, null)) {
            XtSystem system = platform.setupToSystem(XtSetup.CONSUMER_AUDIO);
            XtService service = platform.getService(system);
            if(service == null) return;

            String defaultInput = service.getDefaultDeviceId(false);
            if(defaultInput == null) return;
            try(XtDevice device = service.openDevice(defaultInput)) {
                if(!device.supportsFormat(FORMAT)) return;
                XtBufferSize size = device.getBufferSize(FORMAT);

                System.out.println("Capture interleaved, safe buffers...");
                streamParams = new XtStreamParams(true, CaptureAdvanced::onInterleavedSafeBuffer, CaptureAdvanced::onXRun, CaptureAdvanced::onRunning);
                deviceParams = new XtDeviceStreamParams(streamParams, FORMAT, size.current);
                try(FileOutputStream recording = new FileOutputStream("xt-audio-interleaved-safe.raw");
                    XtStream stream = device.openStream(deviceParams, recording);
                    XtSafeBuffer safe = XtSafeBuffer.register(stream, true)) {
                    runStream(stream);
                }

                System.out.println("Capture interleaved, native buffers...");
                streamParams = new XtStreamParams(true, CaptureAdvanced::onInterleavedNativeBuffer, CaptureAdvanced::onXRun, CaptureAdvanced::onRunning);
                deviceParams = new XtDeviceStreamParams(streamParams, FORMAT, size.current);
                Context context = new Context();
                try(FileOutputStream recording = new FileOutputStream("xt-audio-interleaved-native.raw");
                    XtStream stream = device.openStream(deviceParams, context)) {
                    context.out = recording;
                    context.intermediate = new byte[getBufferSize(CHANNELS.inputs, stream.getFrames())];
                    runStream(stream);
                }

                System.out.println("Capture non-interleaved, safe buffers...");
                streamParams = new XtStreamParams(false, CaptureAdvanced::onNonInterleavedSafeBuffer, CaptureAdvanced::onXRun, CaptureAdvanced::onRunning);
                deviceParams = new XtDeviceStreamParams(streamParams, FORMAT, size.current);
                try(FileOutputStream recording = new FileOutputStream("xt-audio-non-interleaved-safe.raw");
                    XtStream stream = device.openStream(deviceParams, recording);
                    XtSafeBuffer safe = XtSafeBuffer.register(stream, false)) {
                    runStream(stream);
                }

                System.out.println("Capture non-interleaved, native buffers...");
                context = new Context();
                streamParams = new XtStreamParams(false, CaptureAdvanced::onNonInterleavedNativeBuffer, CaptureAdvanced::onXRun, CaptureAdvanced::onRunning);
                deviceParams = new XtDeviceStreamParams(streamParams, FORMAT, size.current);
                try(FileOutputStream recording = new FileOutputStream("xt-audio-non-interleaved-native.raw");
                    XtStream stream = device.openStream(deviceParams, context)) {
                    context.out = recording;
                    context.intermediate = new byte[getBufferSize(1, stream.getFrames())];
                    runStream(stream);
                }
            }
        }
    }
}