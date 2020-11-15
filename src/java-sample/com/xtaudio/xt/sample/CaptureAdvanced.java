package com.xtaudio.xt.sample;

import com.sun.jna.Native;
import com.sun.jna.Pointer;

import com.xtaudio.xt.CoreEnums.XtSample;
import com.xtaudio.xt.CoreEnums.XtSetup;
import com.xtaudio.xt.CoreEnums.XtSystem;
import com.xtaudio.xt.CoreStructs.XtBuffer;
import com.xtaudio.xt.CoreStructs.XtBufferSize;
import com.xtaudio.xt.CoreStructs.XtChannels;
import com.xtaudio.xt.CoreStructs.XtDeviceStreamParams;
import com.xtaudio.xt.CoreStructs.XtFormat;
import com.xtaudio.xt.CoreStructs.XtMix;
import com.xtaudio.xt.CoreStructs.XtStreamParams;
import com.xtaudio.xt.XtAudio;
import com.xtaudio.xt.XtDevice;
import com.xtaudio.xt.XtSafeBuffer;
import com.xtaudio.xt.XtService;
import com.xtaudio.xt.XtStream;
import java.io.FileOutputStream;

public class CaptureAdvanced {

    static class Context {
        byte[] intermediate;
        FileOutputStream out;
    }

    static final XtMix MIX = new XtMix(44100, XtSample.INT24);
    static final XtChannels CHANNELS = new XtChannels(2, 0, 0, 0);
    static final XtFormat FORMAT = new XtFormat(MIX, CHANNELS);

    static void onXRun(int index, Object user) {
        System.out.println("XRun on device " + index + ".");
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

    static void onInterleavedSafeBuffer(XtStream stream, XtBuffer buffer, Object user) throws Exception {
        var out = (FileOutputStream)user;
        XtSafeBuffer safe = XtSafeBuffer.get(stream);
        int bytes = getBufferSize(CHANNELS.inputs, buffer.frames);
        safe.lock(buffer);
        out.write((byte[])safe.getInput(), 0, bytes);
        safe.unlock(buffer);
    }

    static void onInterleavedNativeBuffer(XtStream stream, XtBuffer buffer, Object user) throws Exception {
        var ctx = (Context)user;
        int bytes = getBufferSize(CHANNELS.inputs, buffer.frames);
        buffer.input.read(0, ctx.intermediate, 0, bytes);
        ctx.out.write(ctx.intermediate, 0, bytes);
    }

    static void onNonInterleavedSafeBuffer(XtStream stream, XtBuffer buffer, Object user) throws Exception {
        var out = (FileOutputStream)user;
        XtSafeBuffer safe = XtSafeBuffer.get(stream);
        int size = XtAudio.getSampleAttributes(MIX.sample).size;
        safe.lock(buffer);
        for(int f = 0; f < buffer.frames; f++)
            for(int c = 0; c < CHANNELS.inputs; c++)
                out.write(((byte[][])safe.getInput())[c], f * size, size);
        safe.unlock(buffer);
    }

    static void onNonInterleavedNativeBuffer(XtStream stream, XtBuffer buffer, Object user) throws Exception {
        var ctx = (Context)user;
        int size = XtAudio.getSampleAttributes(MIX.sample).size;
        for(int f = 0; f < buffer.frames; f++)
            for(int c = 0; c < CHANNELS.inputs; c++) {
                Pointer channel = buffer.input.getPointer(c * Native.POINTER_SIZE);
                channel.read(f * size, ctx.intermediate, 0, size);
                ctx.out.write(ctx.intermediate, 0, size);
            }
    }

    public static void main() throws Exception {
        XtStreamParams streamParams;
        XtDeviceStreamParams deviceParams;

        try(AutoCloseable audio = XtAudio.init(null, null, null)) {
            XtSystem system = XtAudio.setupToSystem(XtSetup.CONSUMER_AUDIO);
            XtService service = XtAudio.getService(system);
            if(service == null) return;

            try(XtDevice device = service.openDefaultDevice(false)) {
                if(device == null || !device.supportsFormat(FORMAT)) return;
                XtBufferSize size = device.getBufferSize(FORMAT);

                System.out.println("Capture interleaved, safe buffers...");
                streamParams = new XtStreamParams(true, CaptureAdvanced::onInterleavedSafeBuffer, CaptureAdvanced::onXRun);
                deviceParams = new XtDeviceStreamParams(streamParams, FORMAT, size.current);
                try(FileOutputStream recording = new FileOutputStream("xt-audio-interleaved-safe.raw");
                    XtStream stream = device.openStream(deviceParams, recording);
                    XtSafeBuffer safe = XtSafeBuffer.register(stream, true)) {
                    runStream(stream);
                }

                System.out.println("Capture interleaved, native buffers...");
                streamParams = new XtStreamParams(true, CaptureAdvanced::onInterleavedNativeBuffer, CaptureAdvanced::onXRun);
                deviceParams = new XtDeviceStreamParams(streamParams, FORMAT, size.current);
                Context context = new Context();
                try(FileOutputStream recording = new FileOutputStream("xt-audio-interleaved-native.raw");
                    XtStream stream = device.openStream(deviceParams, context)) {
                    context.out = recording;
                    context.intermediate = new byte[getBufferSize(CHANNELS.inputs, stream.getFrames())];
                    runStream(stream);
                }

                System.out.println("Capture non-interleaved, safe buffers...");
                streamParams = new XtStreamParams(false, CaptureAdvanced::onNonInterleavedSafeBuffer, CaptureAdvanced::onXRun);
                deviceParams = new XtDeviceStreamParams(streamParams, FORMAT, size.current);
                try(FileOutputStream recording = new FileOutputStream("xt-audio-non-interleaved-safe.raw");
                    XtStream stream = device.openStream(deviceParams, recording);
                    XtSafeBuffer safe = XtSafeBuffer.register(stream, false)) {
                    runStream(stream);
                }

                System.out.println("Capture non-interleaved, native buffers...");
                context = new Context();
                streamParams = new XtStreamParams(false, CaptureAdvanced::onNonInterleavedNativeBuffer, CaptureAdvanced::onXRun);
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