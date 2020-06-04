package com.xtaudio.xt.sample;

import com.xtaudio.xt.XtAudio;
import com.xtaudio.xt.XtChannels;
import com.xtaudio.xt.XtDevice;
import com.xtaudio.xt.XtFormat;
import com.xtaudio.xt.XtMix;
import com.xtaudio.xt.XtSample;
import com.xtaudio.xt.XtService;
import com.xtaudio.xt.XtSetup;
import com.xtaudio.xt.XtStream;

public class Aggregate {

    static void readLine() {
        System.out.println("Press any key to continue...");
        System.console().readLine();
    }

    static void xRun(int index, Object user) {
        // Don't do this.
        System.out.println("XRun on device " + index + ", user = " + user + ".");
    }

    static void aggregate(XtStream stream, Object input, Object output, int frames, double time,
            long position, boolean timeValid, long error, Object user) throws Exception {

        if (frames > 0)
            System.arraycopy(input, 0, output, 0, frames * stream.getFormat().inputs);
    }

    public static void main(String[] args) throws Exception {

        XtMix mix = new XtMix(48000, XtSample.INT16);
        XtFormat inputFormat = new XtFormat(mix, 2, 0, 0, 0);
        XtChannels inputChannels = new XtChannels(2, 0, 0, 0);
        XtFormat outputFormat = new XtFormat(mix, 0, 0, 2, 0);
        XtChannels outputChannels = new XtChannels(0, 0, 2, 0);

        try (XtAudio audio = new XtAudio(null, null, null, null)) {

            XtService service = XtAudio.getServiceBySetup(XtSetup.SYSTEM_AUDIO);
            if (service == null)
                return;

            try (XtDevice input = service.openDefaultDevice(false);
                    XtDevice output = service.openDefaultDevice(true)) {
                if (input == null || !input.supportsFormat(inputFormat))
                    return;
                if (output == null || !output.supportsFormat(outputFormat))
                    return;

                try (XtStream stream = service.aggregateStream(
                        new XtDevice[]{input, output},
                        new XtChannels[]{inputChannels, outputChannels},
                        new double[]{30.0, 30.0},
                        2, mix, true, false, output, Aggregate::aggregate, Aggregate::xRun, "user-data")) {
                    stream.start();
                    System.out.println("Streaming aggregate, press any key to continue...");
                    System.console().readLine();
                    stream.stop();
                }
            }
        }
    }
}
