package com.xtaudio.xt.sample;

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

    static void onBuffer(XtStream stream, XtBuffer buffer, Object user) {
        XtSafeBuffer safe = XtSafeBuffer.get(stream);
        safe.lock(buffer);
        float[] output = (float[])safe.getOutput();
        for(int f = 0; f < buffer.frames; f++) output[f] = nextSample();
        safe.unlock(buffer);
    }

    public static void main() throws Exception {
        XtStreamParams streamParams;
        XtDeviceStreamParams deviceParams;

        try(AutoCloseable audio = XtAudio.init(null, null, null)) {
            XtSystem system = XtAudio.setupToSystem(XtSetup.CONSUMER_AUDIO);
            XtService service = XtAudio.getService(system);
            if(service == null) return;

            try(XtDevice device = service.openDefaultDevice(true)) {
                if(device == null || !device.supportsFormat(FORMAT)) return;

                XtBufferSize size = device.getBufferSize(FORMAT);
                streamParams = new XtStreamParams(true, RenderSimple::onBuffer, null);
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