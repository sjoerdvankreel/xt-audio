package com.xtaudio.xt.sample;

import com.xtaudio.xt.*;
import static com.xtaudio.xt.NativeTypes.*;

public class Aggregate {

    static void xRun(int index, Object user) {
        System.out.println("XRun on device " + index + ".");
    }

    static void aggregate(XtStream stream, XtBuffer buffer, Object user) throws Exception {
        XtSafeBuffer safe = XtSafeBuffer.get(stream);
        safe.lock(buffer);
        int count = buffer.frames * stream.getFormat().channels.inputs;
        System.arraycopy(safe.getInput(), 0, safe.getOutput(), 0, count);
        safe.unlock(buffer);
    }

    public static void main(String[] args) throws Exception {
        XtMix mix = new XtMix(48000, XtSample.INT16);
        XtFormat inputFormat = new XtFormat(mix, new XtChannels(2, 0, 0, 0));
        XtFormat outputFormat = new XtFormat(mix, new XtChannels(0, 0, 2, 0));
        try(XtAudio audio = new XtAudio(null, null, null)) {
            var system = XtAudio.setupToSystem(XtSetup.SYSTEM_AUDIO);
            XtService service = XtAudio.getService(system);
            if(service == null) return;
            try(XtDevice input = service.openDefaultDevice(false);
                XtDevice output = service.openDefaultDevice(true)) {
                if(input == null || !input.supportsFormat(inputFormat)) return;
                if(output == null || !output.supportsFormat(outputFormat)) return;
                try(XtStream stream = service.aggregateStream(
                        new XtDevice[]{input, output},
                        new XtChannels[]{inputFormat.channels, outputFormat.channels},
                        new double[]{30.0, 30.0},
                        2, mix, true, output, Aggregate::aggregate, Aggregate::xRun, null);
                    XtSafeBuffer safe = XtSafeBuffer.register(stream, true)) {
                    stream.start();
                    Thread.sleep(2000);
                    stream.stop();
                }
            }
        }
    }
}