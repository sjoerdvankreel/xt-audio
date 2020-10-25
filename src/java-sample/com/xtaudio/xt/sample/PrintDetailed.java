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
            XtService pro = XtAudio.getServiceBySetup(XtSetup.PRO_AUDIO);
            if (pro != null)
                System.out.println("Pro Audio: " + pro);
            XtService system = XtAudio.getServiceBySetup(XtSetup.SYSTEM_AUDIO);
            if (system != null)
                System.out.println("System Audio: " + system);
            XtService consumer = XtAudio.getServiceBySetup(XtSetup.CONSUMER_AUDIO);
            if (consumer != null)
                System.out.println("Consumer Audio: " + consumer);

            for (int s = 0; s < XtAudio.getServiceCount(); s++) {

                XtService service = XtAudio.getServiceByIndex(s);
                System.out.println("Service " + service + ":");
                System.out.println("  System: " + service.getSystem());
                System.out.println("  Device count: " + service.getDeviceCount());
                System.out.println("  Capabilities: " + XtCapabilities.toString(service.getCapabilities()));

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
                    System.out.println("    System: " + device.getSystem());
                    if (mix.isPresent())
                        System.out.println("    Current mix: " + mix.get().rate + " " + mix.get().sample);
                    System.out.println("    Input channels: " + device.getChannelCount(false));
                    System.out.println("    Output channels: " + device.getChannelCount(true));
                    System.out.println("    Interleaved access: " + device.supportsAccess(true));
                    System.out.println("    Non-interleaved access: " + device.supportsAccess(false));
                }
            }
        } catch (XtException e) {

            XtErrorInfo info = XtAudio.getErrorInfo(e.getError());
            System.out.printf("Error: system %s, fault %s, cause %s, text %s.\n",
                info.system, info.fault, info.cause, info.text);
        }
    }
}
