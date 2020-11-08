package com.xtaudio.xt.sample;

import java.io.FileOutputStream;

import com.sun.jna.*;
import com.xtaudio.xt.*;
import static com.xtaudio.xt.NativeTypes.*;

public class CaptureSimple {

    static final int SAMPLE_SIZE = 3;
    static final XtMix MIX = new XtMix(44100, XtSample.INT24);
    static final XtChannels CHANNELS = new XtChannels(1, 0, 0, 0);
    static final XtFormat FORMAT = new XtFormat(MIX, CHANNELS);

    static void capture(Pointer stream, XtBuffer buffer, Pointer user) throws Exception {
        XtAdapter adapter = XtAdapter.get(stream);
        adapter.lockBuffer(buffer);
        // Don't do this.
        if(buffer.frames > 0)
            ((FileOutputStream)adapter.getUser()).write((byte[])adapter.getInput(), 0, buffer.frames * SAMPLE_SIZE);
        adapter.unlockBuffer(buffer);
    }

    public static void main(String[] args) throws Exception {

        try(XtAudio audio = new XtAudio(null, null, null)) {
            XtSystem system = XtAudio.setupToSystem(XtSetup.CONSUMER_AUDIO);
            XtService service = XtAudio.getService(system);
            if(service == null) return;
            try(XtDevice device = service.openDefaultDevice(false)) {
                if(device == null || !device.supportsFormat(FORMAT)) return;
                XtBufferSize size = device.getBufferSize(FORMAT);
                try(FileOutputStream recording = new FileOutputStream("xt-audio.raw");
                    XtStream stream = device.openStream(FORMAT, true, size.current, CaptureSimple::capture, null);
                    XtAdapter adapter = XtAdapter.register(stream, true, recording)) {
                    stream.start();
                    Thread.sleep(1000);
                    stream.stop();
                }
            }
        }
    }
}