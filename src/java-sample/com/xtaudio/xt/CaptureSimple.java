package com.xtaudio.xt;

import java.io.FileOutputStream;

public class CaptureSimple {

    static final int SAMPLE_SIZE = 3;
    static final XtFormat FORMAT = new XtFormat(new XtMix(44100, XtSample.INT24), 1, 0, 0, 0);

    static void capture(XtStream stream, Object input, Object output, int frames, double time,
            long position, boolean timeValid, long error, Object user) throws Exception {

        if (frames > 0)
            // Don't do this.
            ((FileOutputStream) user).write((byte[]) input, 0, frames * SAMPLE_SIZE);
    }

    public static void main(String[] args) throws Exception {

        try (XtAudio audio = new XtAudio(null, null, null, null)) {
            XtService service = XtAudio.getServiceBySetup(XtSetup.CONSUMER_AUDIO);
            try (XtDevice device = service.openDefaultDevice(false)) {
                if (device != null && device.supportsFormat(FORMAT)) {

                    XtBuffer buffer = device.getBuffer(FORMAT);
                    try (FileOutputStream recording = new FileOutputStream("xt-audio.raw");
                            XtStream stream = device.openStream(FORMAT, true, false,
                                    buffer.current, CaptureSimple::capture, null, recording)) {
                        stream.start();
                        Thread.sleep(1000);
                        stream.stop();
                    }
                }
            }
        }
    }
}
