package com.xtaudio.xt;

import com.sun.jna.Pointer;
import java.util.Random;

public class RenderDefaultOutput {

    static final Random RANDOM = new Random();

    static void render(XtStream stream, Object input, Object output, int frames,
            double time, long position, boolean timeValid, long error, Object user) {

        XtFormat format = stream.getFormat();
        short[] buffer = (short[]) output;
        for (int f = 0; f < frames; f++)
            for (int c = 0; c < format.outputs; c++) {
                double noise = RANDOM.nextDouble() * 2.0 - 1.0;
                buffer[f * format.outputs + c] = (short) (noise * Short.MAX_VALUE);
            }
    }

    public static void main(String[] args) {

        try (XtAudio audio = new XtAudio(null, Pointer.NULL, null, null)) {
            XtService service = XtAudio.getServiceBySetup(XtSetup.CONSUMER_AUDIO);
            try (XtDevice device = service.openDefaultDevice(true)) {

                if (device == null) {
                    System.out.println("No default device found.");
                    return;
                }

                XtFormat format = new XtFormat(new XtMix(44100, XtSample.INT16), 0, 0, 2, 0);
                if (!device.supportsFormat(format)) {
                    System.out.println("Format not supported.");
                    return;
                }

                XtBuffer buffer = device.getBuffer(format);
                try (XtStream stream = device.openStream(format, true, false, buffer.current, RenderDefaultOutput::render, "user-data")) {
                    stream.start();
                    Thread.sleep(1000);
                    stream.stop();
                } catch (InterruptedException e) {
                    throw new RuntimeException(e);
                }
            }
        }
    }
}
