package com.xtaudio.xt;

public class PrintDetailed {

    static void onFatal() {
        System.out.println("Fatal error.");
    }

    static void onTrace(XtLevel level, String message) {
        if (level != XtLevel.INFO)
            System.out.println("-- " + level + ": " + message);
    }

    public static void main(String[] args) {

        try (XtAudio audio = new XtAudio("Sample", null, PrintDetailed::onTrace, PrintDetailed::onFatal)) {

            System.out.println("Win32: " + XtAudio.isWin32());
            System.out.println("Version: " + XtAudio.getVersion());
            System.out.println("Pro Audio: " + XtAudio.getServiceBySetup(XtSetup.PRO_AUDIO).getName());
            System.out.println("System Audio: " + XtAudio.getServiceBySetup(XtSetup.SYSTEM_AUDIO).getName());
            System.out.println("Consumer Audio: " + XtAudio.getServiceBySetup(XtSetup.CONSUMER_AUDIO).getName());

            for (int s = 0; s < XtAudio.getServiceCount(); s++) {

                XtService service = XtAudio.getServiceByIndex(s);
                System.out.println("Service " + service.getName() + ":");
                System.out.println("  System: " + service.getSystem());
                System.out.println("  Device count: " + service.getDeviceCount());
                System.out.println("  Capabilities: " + XtPrint.capabilitiesToString(service.getCapabilities()));

                try (XtDevice defaultInput = service.openDefaultDevice(false)) {
                    System.out.println("  Default input: " + defaultInput);
                }
                try (XtDevice defaultOutput = service.openDefaultDevice(true)) {
                    System.out.println("  Default output: " + defaultOutput);
                }

                for (int d = 0; d < service.getDeviceCount(); d++)
                    try (XtDevice device = service.openDevice(d)) {

                        System.out.println("  Device " + device.getName() + ":");
                        System.out.println("    System: " + device.getSystem());
                        System.out.println("    Current mix: " + device.getMix());
                        System.out.println("    Input channels: " + device.getChannelCount(false));
                        System.out.println("    Output channels: " + device.getChannelCount(true));
                        System.out.println("    Interleaved access: " + device.supportsAccess(true));
                        System.out.println("    Non-interleaved access: " + device.supportsAccess(false));
                    }
            }
        } catch (XtException e) {

            System.out.printf("Error: system %s, fault %s, cause %s, text %s, message: %s.\n",
                    XtException.getSystem(e.getError()),
                    XtException.getFault(e.getError()),
                    XtException.getCause(e.getError()),
                    XtException.getText(e.getError()),
                    e.toString());
        }
    }
}
