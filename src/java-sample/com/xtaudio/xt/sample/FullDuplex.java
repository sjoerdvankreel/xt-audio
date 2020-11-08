package com.xtaudio.xt.sample;

import com.sun.jna.*;
import com.xtaudio.xt.*;
import static com.xtaudio.xt.NativeTypes.*;

public class FullDuplex {

    static void callback(Pointer stream, XtBuffer buffer, Pointer user) throws Exception {
        XtAdapter adapter = XtAdapter.get(stream);
        adapter.lockBuffer(buffer);
        System.arraycopy(adapter.getInput(), 0, adapter.getOutput(), 0, buffer.frames * 2);
        adapter.unlockBuffer(buffer);
    }

    public static void main(String[] args) throws Exception {
        XtFormat format;
        XtFormat int44100 = new XtFormat(new XtMix(44100, XtSample.INT32), new XtChannels(2, 0, 2, 0));
        XtFormat int48000 = new XtFormat(new XtMix(48000, XtSample.INT32), new XtChannels(2, 0, 2, 0));
        XtFormat float44100 = new XtFormat(new XtMix(44100, XtSample.FLOAT32), new XtChannels(2, 0, 2, 0));
        XtFormat float48000 = new XtFormat(new XtMix(48000, XtSample.FLOAT32), new XtChannels(2, 0, 2, 0));
        try(XtAudio audio = new XtAudio(null, null, null)) {
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
                try(XtStream stream = device.openStream(format, true, size.min, FullDuplex::callback, null);
                    XtAdapter adapter = XtAdapter.register(stream, true, null)) {
                    stream.start();
                    System.out.println("Streaming full-duplex, press any key to continue...");
                    System.console().readLine();
                    stream.stop();
                }
            }
        }
    }
}