package com.xtaudio.xt.sample;

import com.xtaudio.xt.XtAudio;
import com.xtaudio.xt.XtDevice;
import com.xtaudio.xt.XtException;
import com.xtaudio.xt.XtService;
import static com.xtaudio.xt.NativeTypes.*;
import java.util.Optional;

public class PrintDetailed {

    static void onFatal() {
        System.out.println("Fatal error.");
    }

    static void onTrace(XtLevel level, String message) {
        if (level != XtLevel.INFO)
            System.out.println("-- " + level + ": " + message);
    }

    public static void main(String[] args) {

        try ( XtAudio audio = new XtAudio("Sample", null, PrintDetailed::onTrace, PrintDetailed::onFatal)) {

            XtVersion version = XtAudio.getVersion();
            System.out.println("Version: " + version.major + "." + version.minor);
            var pro = XtAudio.setupToSystem(XtSetup.PRO_AUDIO);
            System.out.println("Pro Audio: " + pro + " (" + (XtAudio.getService(pro) != null) + ")");
            var system = XtAudio.setupToSystem(XtSetup.SYSTEM_AUDIO);
            System.out.println("System Audio: " + system + " (" + (XtAudio.getService(system) != null) + ")");
            var consumer = XtAudio.setupToSystem(XtSetup.CONSUMER_AUDIO);
            System.out.println("Consumer Audio: " + consumer + " (" + (XtAudio.getService(consumer) != null) + ")");

            for (var s: XtAudio.getSystems()) {
                XtService service = XtAudio.getService(s);
                System.out.println("System " + s + ":");
                System.out.println("  Device count: " + service.getDeviceCount());
                System.out.println("  Capabilities: " + service.getCapabilities());

                try ( XtDevice defaultInput = service.openDefaultDevice(false)) {
                    System.out.println("  Default input: " + defaultInput);
                }
                try ( XtDevice defaultOutput = service.openDefaultDevice(true)) {
                    System.out.println("  Default output: " + defaultOutput);
                }

                for (int d = 0; d < service.getDeviceCount(); d++)
                    try ( XtDevice device = service.openDevice(d)) {
                    Optional<XtMix> mix = device.getMix();
                    System.out.println("  Device " + device + ":");
                    System.out.println("    Input channels: " + device.getChannelCount(false));
                    System.out.println("    Output channels: " + device.getChannelCount(true));
                    System.out.println("    Interleaved access: " + device.supportsAccess(true));
                    System.out.println("    Non-interleaved access: " + device.supportsAccess(false));
                    if (mix.isPresent()) System.out.println("    Current mix: " + mix.get().rate + " " + mix.get().sample);
                }
            }
        } catch (XtException e) {

            XtErrorInfo info = XtAudio.getErrorInfo(e.getError());
            System.out.printf("Error: system %s, fault %s, cause %s, text %s.\n",
                info.system, info.fault, info.cause, info.text);
        }
    }
}
