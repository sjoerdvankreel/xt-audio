package com.xtaudio.xt;

public class RenderSimple {

    static double phase = 0.0;
    static final double FREQUENCY = 660.0;
    static final XtFormat FORMAT = new XtFormat(new XtMix(44100, XtSample.FLOAT32), 0, 0, 1, 0);

    static void render(XtStream stream, Object input, Object output, int frames,
            double time, long position, boolean timeValid, long error, Object user) {

        for (int f = 0; f < frames; f++) {
            phase += FREQUENCY / FORMAT.mix.rate;
            if (phase >= 1.0)
                phase = -1.0;
            ((float[]) output)[f] = (float) Math.sin(phase * Math.PI);
        }
    }

    public static void main(String[] args) throws Exception {

        try (XtAudio audio = new XtAudio(null, null, null, null)) {
            XtService service = XtAudio.getServiceBySetup(XtSetup.CONSUMER_AUDIO);
            try (XtDevice device = service.openDefaultDevice(true)) {
                if (device != null && device.supportsFormat(FORMAT)) {

                    XtBuffer buffer = device.getBuffer(FORMAT);
                    try (XtStream stream = device.openStream(FORMAT, true, false,
                            buffer.current, RenderSimple::render, null, null)) {
                        stream.start();
                        Thread.sleep(1000);
                        stream.stop();
                    }
                }
            }
        }
    }
}
