package xt.sample;

import xt.audio.Enums.XtSample;
import xt.audio.Enums.XtServiceCaps;
import xt.audio.Enums.XtSetup;
import xt.audio.Enums.XtSystem;
import xt.audio.Structs.XtAggregateDeviceParams;
import xt.audio.Structs.XtAggregateStreamParams;
import xt.audio.Structs.XtBuffer;
import xt.audio.Structs.XtChannels;
import xt.audio.Structs.XtFormat;
import xt.audio.Structs.XtMix;
import xt.audio.Structs.XtStreamParams;
import xt.audio.XtAudio;
import xt.audio.XtDevice;
import xt.audio.XtPlatform;
import xt.audio.XtSafeBuffer;
import xt.audio.XtService;
import xt.audio.XtStream;

public class Aggregate {

    // Normally don't do I/O in the callback.
    static void onXRun(XtStream stream, int index, Object user) {
        System.out.println("XRun on device " + index + ".");
    }

    static void onRunning(XtStream stream, boolean running, long error, Object user) {
        String evt = running? "Started": "Stopped";
        System.out.println("Stream event: " + evt + ", new state: " + stream.isRunning() + ".");
        if(error != 0) System.out.println(XtAudio.getErrorInfo(error).toString());
    }

    static int onBuffer(XtStream stream, XtBuffer buffer, Object user) throws Exception {
        XtSafeBuffer safe = XtSafeBuffer.get(stream);
        safe.lock(buffer);
        int count = buffer.frames * stream.getFormat().channels.inputs;
        System.arraycopy(safe.getInput(), 0, safe.getOutput(), 0, count);
        safe.unlock(buffer);
        return 0;
    }

    public static void main() throws Exception {

        XtAggregateStreamParams aggregateParams;
        XtMix mix = new XtMix(48000, XtSample.INT16);
        XtFormat inputFormat = new XtFormat(mix, new XtChannels(2, 0, 0, 0));
        XtFormat outputFormat = new XtFormat(mix, new XtChannels(0, 0, 2, 0));

        try(XtPlatform platform = XtAudio.init(null, null)) {
            XtSystem system = platform.setupToSystem(XtSetup.SYSTEM_AUDIO);
            XtService service = platform.getService(system);
            if(service == null || !service.getCapabilities().contains(XtServiceCaps.AGGREGATION)) return;

            String defaultInput = service.getDefaultDeviceId(false);
            String defaultOutput = service.getDefaultDeviceId(true);
            if(defaultInput == null || defaultOutput == null) return;

            try(XtDevice input = service.openDevice(defaultInput);
                XtDevice output = service.openDevice(defaultOutput)) {
                if(!input.supportsFormat(inputFormat)) return;
                if(!output.supportsFormat(outputFormat)) return;

                XtAggregateDeviceParams[] deviceParams = new XtAggregateDeviceParams[2];
                deviceParams[0] = new XtAggregateDeviceParams(input, inputFormat.channels, 30.0);
                deviceParams[1] = new XtAggregateDeviceParams(output, outputFormat.channels, 30.0);
                XtStreamParams streamParams = new XtStreamParams(true, Aggregate::onBuffer, Aggregate::onXRun, Aggregate::onRunning);
                aggregateParams = new XtAggregateStreamParams(streamParams, deviceParams, 2, mix, output);
                try(XtStream stream = service.aggregateStream(aggregateParams, null);
                    XtSafeBuffer safe = XtSafeBuffer.register(stream, true)) {
                    stream.start();
                    Thread.sleep(2000);
                    stream.stop();
                }
            }
        }
    }
}