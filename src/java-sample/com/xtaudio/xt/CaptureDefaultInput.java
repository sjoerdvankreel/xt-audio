package com.xtaudio.xt;

import com.sun.jna.Pointer;
import java.io.FileOutputStream;
import java.io.IOException;

public class CaptureDefaultInput {

    static void capture(XtStream stream, Object input, Object output, int frames,
            double time, long position, boolean timeValid, long error, Object user) {

        XtFormat format = stream.getFormat();
        int sampleSize = XtAudio.getSampleAttributes(format.mix.sample).size;
        int bufferSize = frames * format.inputs * sampleSize;

        if (frames != 0)
            try {
                // Don't do this.
                ((FileOutputStream) user).write(((byte[]) input), 0, bufferSize);
            } catch (IOException e) {
                throw new RuntimeException(e);
            }
    }

    public static void main(String[] args) {

        try (XtAudio audio = new XtAudio(null, Pointer.NULL, null, null)) {
            XtService service = XtAudio.getServiceBySetup(XtSetup.CONSUMER_AUDIO);
            try (XtDevice device = service.openDefaultDevice(false)) {

                if (device == null) {
                    System.out.println("No default device found.");
                    return;
                }

                XtFormat format = new XtFormat(new XtMix(44100, XtSample.INT24), 2, 0, 0, 0);
                if (!device.supportsFormat(format)) {
                    System.out.println("Format not supported.");
                    return;
                }

                XtBuffer buffer = device.getBuffer(format);
                try (FileOutputStream recording = new FileOutputStream("xt-audio.raw");
                        XtStream stream = device.openStream(format, true, false, buffer.current, CaptureDefaultInput::capture, recording)) {
                    stream.start();
                    Thread.sleep(1000);
                    stream.stop();
                } catch (Exception e) {
                    throw new RuntimeException(e);
                }
            }
        }
    }
}
