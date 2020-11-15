package com.xtaudio.xt.sample;

import java.io.FileOutputStream;
import com.xtaudio.xt.*;
import static com.xtaudio.xt.NativeTypes.*;

public class CaptureSimple {

    static final XtMix MIX = new XtMix(44100, XtSample.INT24);
    static final XtChannels CHANNELS = new XtChannels(1, 0, 0, 0);
    static final XtFormat FORMAT = new XtFormat(MIX, CHANNELS);

    static void onBuffer(XtStream stream, XtBuffer buffer, Object user) throws Exception {
        var output = (FileOutputStream)user;
        XtSafeBuffer safe = XtSafeBuffer.get(stream);
        safe.lock(buffer);
        var input = (byte[])safe.getInput();
        int size = XtAudio.getSampleAttributes(MIX.sample).size;
        output.write(input, 0, buffer.frames * size);
        safe.unlock(buffer);
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
                streamParams = new XtStreamParams(true, CaptureSimple::onBuffer, null);
                deviceParams = new XtDeviceStreamParams(streamParams, FORMAT, size.current);
                try(FileOutputStream recording = new FileOutputStream("xt-audio.raw");
                    XtStream stream = device.openStream(deviceParams, recording);
                    XtSafeBuffer safe = XtSafeBuffer.register(stream, true)) {
                    stream.start();
                    Thread.sleep(2000);
                    stream.stop();
                }
            }
        }
    }
}