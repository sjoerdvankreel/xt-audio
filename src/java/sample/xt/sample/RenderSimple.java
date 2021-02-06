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

public class RenderSimple {

    static float _phase = 0.0f;
    static final float FREQUENCY = 440.0f;
    static final XtMix MIX = new XtMix(44100, XtSample.FLOAT32);
    static final XtChannels CHANNELS = new XtChannels(0, 0, 1, 0);
    static final XtFormat FORMAT = new XtFormat(MIX, CHANNELS);

    static float nextSample() {
        _phase += FREQUENCY / FORMAT.mix.rate;
        if(_phase >= 1.0f) _phase = -1.0f;
        return (float)Math.sin(2.0 * _phase * Math.PI);
    }

    static int onBuffer(XtStream stream, XtBuffer buffer, Object user) {
        XtSafeBuffer safe = XtSafeBuffer.get(stream);
        safe.lock(buffer);
        float[] output = (float[])safe.getOutput();
        for(int f = 0; f < buffer.frames; f++) output[f] = nextSample();
        safe.unlock(buffer);
        return 0;
    }

    public static void main() throws Exception {
        XtStreamParams streamParams;
        XtDeviceStreamParams deviceParams;

        try(XtPlatform platform = XtAudio.init(null, null)) {
            XtSystem system = platform.setupToSystem(XtSetup.CONSUMER_AUDIO);
            XtService service = platform.getService(system);
            if(service == null) return;

            String defaultOutput = service.getDefaultDeviceId(true);
            if(defaultOutput == null) return;
            try(XtDevice device = service.openDevice(defaultOutput)) {
                if(!device.supportsFormat(FORMAT)) return;

                XtBufferSize size = device.getBufferSize(FORMAT);
                streamParams = new XtStreamParams(true, RenderSimple::onBuffer, null, null);
                deviceParams = new XtDeviceStreamParams(streamParams, FORMAT, size.current);
                try(XtStream stream = device.openStream(deviceParams, null);
                    XtSafeBuffer safe = XtSafeBuffer.register(stream, true)) {
                    stream.start();
                    Thread.sleep(2000);
                    stream.stop();
                }
            }
        }
    }
}