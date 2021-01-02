package xt.sample;

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

public class CaptureSimple {

    static final XtMix MIX = new XtMix(44100, XtSample.INT24);
    static final XtChannels CHANNELS = new XtChannels(1, 0, 0, 0);
    static final XtFormat FORMAT = new XtFormat(MIX, CHANNELS);

    // Normally don't do I/O in the callback.
    static int onBuffer(XtStream stream, XtBuffer buffer, Object user) throws Exception {
        var output = (FileOutputStream)user;
        XtSafeBuffer safe = XtSafeBuffer.get(stream);
        safe.lock(buffer);
        var input = (byte[])safe.getInput();
        int size = XtAudio.getSampleAttributes(MIX.sample).size;
        output.write(input, 0, buffer.frames * size);
        safe.unlock(buffer);
        return 0;
    }

    public static void main() throws Exception {
        XtStreamParams streamParams;
        XtDeviceStreamParams deviceParams;

        try(XtPlatform platform = XtAudio.init(null, null, null)) {
            XtSystem system = platform.setupToSystem(XtSetup.CONSUMER_AUDIO);
            XtService service = platform.getService(system);
            if(service == null) return;

            String defaultInput = service.getDefaultDeviceId(false);
            if(defaultInput == null) return;
            try(XtDevice device = service.openDevice(defaultInput)) {
                if(!device.supportsFormat(FORMAT)) return;

                XtBufferSize size = device.getBufferSize(FORMAT);
                streamParams = new XtStreamParams(true, CaptureSimple::onBuffer, null, null);
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