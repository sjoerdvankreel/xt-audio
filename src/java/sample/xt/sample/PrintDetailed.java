package xt.sample;

import xt.audio.Enums.XtSetup;
import xt.audio.Enums.XtSystem;
import xt.audio.Structs.XtLocation;
import xt.audio.Structs.XtMix;
import xt.audio.Structs.XtVersion;
import xt.audio.XtAudio;
import xt.audio.XtDevice;
import xt.audio.XtException;
import xt.audio.XtPlatform;
import xt.audio.XtService;
import java.util.Optional;

public class PrintDetailed {

    static void onError(XtLocation location, String message) {
        System.out.println(location + ": " + message);
    }

    public static void main() throws Exception {
        try(XtPlatform platform = XtAudio.init("Sample", null, PrintDetailed::onError)) {
            XtVersion version = XtAudio.getVersion();
            System.out.println("Version: " + version.major + "." + version.minor);
            XtSystem pro = XtAudio.setupToSystem(XtSetup.PRO_AUDIO);
            System.out.println("Pro Audio: " + pro + " (" + (platform.getService(pro) != null) + ")");
            XtSystem system = XtAudio.setupToSystem(XtSetup.SYSTEM_AUDIO);
            System.out.println("System Audio: " + system + " (" + (platform.getService(system) != null) + ")");
            XtSystem consumer = XtAudio.setupToSystem(XtSetup.CONSUMER_AUDIO);
            System.out.println("Consumer Audio: " + consumer + " (" + (platform.getService(consumer) != null) + ")");

            for(XtSystem s: platform.getSystems()) {
                XtService service = platform.getService(s);
                System.out.println("System " + s + ":");
                System.out.println("  Device count: " + service.getDeviceCount());
                System.out.println("  Capabilities: " + service.getCapabilities());
                try(XtDevice defaultInput = service.openDefaultDevice(false)) {
                    System.out.println("  Default input: " + defaultInput);
                }
                try(XtDevice defaultOutput = service.openDefaultDevice(true)) {
                    System.out.println("  Default output: " + defaultOutput);
                }

                for(int d = 0; d < service.getDeviceCount(); d++)
                    try(XtDevice device = service.openDevice(d)) {
                        Optional<XtMix> mix = device.getMix();
                        System.out.println("  Device " + device + ":");
                        System.out.println("    Input channels: " + device.getChannelCount(false));
                        System.out.println("    Output channels: " + device.getChannelCount(true));
                        System.out.println("    Interleaved access: " + device.supportsAccess(true));
                        System.out.println("    Non-interleaved access: " + device.supportsAccess(false));
                        if(mix.isPresent())
                            System.out.println("    Current mix: " + mix.get().rate + " " + mix.get().sample);
                    } catch(XtException e) {
                        System.out.println(XtAudio.getErrorInfo(e.getError()));
                    }
            }
        } catch(XtException e) {
            System.out.println(XtAudio.getErrorInfo(e.getError()));
        }
    }
}