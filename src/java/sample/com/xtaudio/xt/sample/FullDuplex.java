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

public class FullDuplex {

    static void onBuffer(XtStream stream, XtBuffer buffer, Object user) throws Exception {
        XtSafeBuffer safe = XtSafeBuffer.get(stream);
        safe.lock(buffer);
        System.arraycopy(safe.getInput(), 0, safe.getOutput(), 0, buffer.frames * 2);
        safe.unlock(buffer);
    }

    public static void main() throws Exception {
        XtFormat format;
        XtStreamParams streamParams;
        XtDeviceStreamParams deviceParams;
        XtFormat int44100 = new XtFormat(new XtMix(44100, XtSample.INT32), new XtChannels(2, 0, 2, 0));
        XtFormat int48000 = new XtFormat(new XtMix(48000, XtSample.INT32), new XtChannels(2, 0, 2, 0));
        XtFormat float44100 = new XtFormat(new XtMix(44100, XtSample.FLOAT32), new XtChannels(2, 0, 2, 0));
        XtFormat float48000 = new XtFormat(new XtMix(48000, XtSample.FLOAT32), new XtChannels(2, 0, 2, 0));

        try(AutoCloseable audio = XtAudio.init(null, null, null)) {
            XtSystem system = XtAudio.setupToSystem(XtSetup.PRO_AUDIO);
            XtService service = XtAudio.getService(system);
            if(service == null) return;

            try(XtDevice device = service.openDefaultDevice(true)) {
                if(device == null) return;
                if(device.supportsFormat(int44100)) format = int44100;
                else if(device.supportsFormat(int48000)) format = int48000;
                else if(device.supportsFormat(float44100)) format = float44100;
                else if(device.supportsFormat(float48000)) format = float48000;
                else return;

                XtBufferSize size = device.getBufferSize(format);
                streamParams = new XtStreamParams(true, FullDuplex::onBuffer, null);
                deviceParams = new XtDeviceStreamParams(streamParams, format, size.current);
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