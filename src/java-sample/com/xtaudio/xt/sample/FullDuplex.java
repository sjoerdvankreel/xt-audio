package com.xtaudio.xt.sample;

import com.xtaudio.xt.XtAudio;
import com.xtaudio.xt.XtDevice;
import com.xtaudio.xt.XtService;
import com.xtaudio.xt.XtStream;
import static com.xtaudio.xt.NativeTypes.*;

public class FullDuplex {

    static void callback(XtStream stream, Object input, Object output, int frames,
            double time, long position, boolean timeValid, long error, Object user) throws Exception {
        System.arraycopy(input, 0, output, 0, frames * 2);
    }

    public static void main(String[] args) throws Exception {

        XtFormat format;
        XtFormat int44100 = new XtFormat(new XtMix(44100, XtSample.INT32), new XtChannels(2, 0, 2, 0));
        XtFormat int48000 = new XtFormat(new XtMix(48000, XtSample.INT32), new XtChannels(2, 0, 2, 0));
        XtFormat float44100 = new XtFormat(new XtMix(44100, XtSample.FLOAT32), new XtChannels(2, 0, 2, 0));
        XtFormat float48000 = new XtFormat(new XtMix(48000, XtSample.FLOAT32), new XtChannels(2, 0, 2, 0));

        try (XtAudio audio = new XtAudio(null, null, null, null)) {

            XtService service = XtAudio.getServiceBySetup(XtSetup.PRO_AUDIO);
            if (service == null)
                return;

            try (XtDevice device = service.openDefaultDevice(true)) {
                if (device == null)
                    return;

                if (device.supportsFormat(int44100))
                    format = int44100;
                else if (device.supportsFormat(int48000))
                    format = int48000;
                else if (device.supportsFormat(float44100))
                    format = float44100;
                else if (device.supportsFormat(float48000))
                    format = float48000;
                else
                    return;

                XtBuffer buffer = device.getBuffer(format);
                try (XtStream stream = device.openStream(format, true, false,
                        buffer.min, FullDuplex::callback, null, null)) {
                    stream.start();
                    System.out.println("Streaming full-duplex, press any key to continue...");
                    System.console().readLine();
                    stream.stop();
                }
            }
        }
    }
}
