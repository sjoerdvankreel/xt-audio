package com.xtaudio.xt.sample;

import com.sun.jna.*;
import com.xtaudio.xt.*;
import static com.xtaudio.xt.NativeTypes.*;

public class Aggregate {

    static void readLine() {
        System.out.println("Press any key to continue...");
        System.console().readLine();
    }

    static void xRun(int index, Pointer user) {
        // Don't do this.
        System.out.println("XRun on device " + index + ".");
    }

    static void aggregate(Pointer stream, XtBuffer buffer, Pointer user) throws Exception {
        XtAdapter adapter = XtAdapter.get(stream);
        adapter.lockBuffer(buffer);
        if(buffer.frames > 0)
            System.arraycopy(adapter.getInput(), 0, adapter.getOutput(), 0, buffer.frames * adapter.getStream().getFormat().channels.inputs);
        adapter.unlockBuffer(buffer);
    }

    public static void main(String[] args) throws Exception {

        XtMix mix = new XtMix(48000, XtSample.INT16);
        XtFormat inputFormat = new XtFormat(mix, new XtChannels(2, 0, 0, 0));
        XtFormat outputFormat = new XtFormat(mix, new XtChannels(0, 0, 2, 0));

        try(XtAudio audio = new XtAudio(null, null, null)) {

            var system = XtAudio.setupToSystem(XtSetup.SYSTEM_AUDIO);
            XtService service = XtAudio.getService(system);
            if(service == null)
                return;

            try(XtDevice input = service.openDefaultDevice(false);
                XtDevice output = service.openDefaultDevice(true)) {
                if(input == null || !input.supportsFormat(inputFormat))
                    return;
                if(output == null || !output.supportsFormat(outputFormat))
                    return;

                try(XtStream stream = service.aggregateStream(
                        new XtDevice[]{input, output},
                        new XtChannels[]{inputFormat.channels, outputFormat.channels},
                        new double[]{30.0, 30.0},
                        2, mix, true, output, Aggregate::aggregate, Aggregate::xRun);
                    XtAdapter adapter = XtAdapter.register(stream, true, null)) {
                    stream.start();
                    System.out.println("Streaming aggregate, press any key to continue...");
                    System.console().readLine();
                    stream.stop();
                }
            }
        }
    }
}