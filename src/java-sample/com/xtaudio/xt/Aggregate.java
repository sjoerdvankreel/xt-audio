package com.xtaudio.xt;

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
            try (XtDevice input = service.openDefaultDevice(false);
                    XtDevice output = service.openDefaultDevice(true)) {
                if (input != null && input.supportsFormat(inputFormat)
                        && output != null && output.supportsFormat(outputFormat)) {
                    XtBuffer buffer = input.getBuffer(inputFormat);
                    try (XtStream stream = service.aggregateStream(
                            new XtDevice[]{input, output},
                            new XtChannels[]{inputChannels, outputChannels},
                            new double[]{buffer.min, buffer.min},
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
}
