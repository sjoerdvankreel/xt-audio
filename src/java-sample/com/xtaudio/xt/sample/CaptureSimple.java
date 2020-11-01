package com.xtaudio.xt.sample;

import com.xtaudio.xt.XtAudio;
import com.xtaudio.xt.XtDevice;
import com.xtaudio.xt.XtService;
import com.xtaudio.xt.XtStream;
import static com.xtaudio.xt.NativeTypes.*;
import java.io.FileOutputStream;

public class CaptureSimple {

    static final int SAMPLE_SIZE = 3;
    static final XtFormat FORMAT = new XtFormat(new XtMix(44100, XtSample.INT24), new XtChannels(1, 0, 0, 0));

    static void capture(XtStream stream, Object input, Object output, int frames, double time,
            long position, boolean timeValid, long error, Object user) throws Exception {
        // Don't do this.
        if (frames > 0)
            ((FileOutputStream) user).write((byte[]) input, 0, frames * SAMPLE_SIZE);
    }

    public static void main(String[] args) throws Exception {

        try (XtAudio audio = new XtAudio(null, null, null)) {

            var system = XtAudio.setupToSystem(XtSetup.CONSUMER_AUDIO);
            XtService service = XtAudio.getService(system);
            if (service == null)
                return;

            try (XtDevice device = service.openDefaultDevice(false)) {
                if (device == null || !device.supportsFormat(FORMAT))
                    return;

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
