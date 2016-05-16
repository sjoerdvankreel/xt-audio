using System;

namespace Xt {

    public static class PrintDetailedInfo {

        static void OnFatal() {
            Console.WriteLine("Fatal error.");
        }

        static void OnTrace(XtLevel level, string message) {
            if (level != XtLevel.Info)
                Console.WriteLine("-- " + level + ": " + message);
        }

        [STAThread]
        public static void Main(string[] args) {

            try {
                using (XtAudio audio = new XtAudio("Sample", IntPtr.Zero, OnTrace, OnFatal)) {
                    Console.WriteLine("Win32: " + XtAudio.IsWin32());
                    Console.WriteLine("Version: " + XtAudio.GetVersion());
                    Console.WriteLine("Pro Audio: " + XtAudio.GetServiceBySetup(XtSetup.ProAudio).GetName());
                    Console.WriteLine("System Audio: " + XtAudio.GetServiceBySetup(XtSetup.SystemAudio).GetName());
                    Console.WriteLine("Consumer Audio: " + XtAudio.GetServiceBySetup(XtSetup.ConsumerAudio).GetName());

                    for (int s = 0; s < XtAudio.GetServiceCount(); s++) {
                        XtService service = XtAudio.GetServiceByIndex(s);
                        Console.WriteLine("Service " + service.GetName() + ":");
                        Console.WriteLine("  System: " + service.GetSystem());
                        Console.WriteLine("  Device count: " + service.GetDeviceCount());
                        Console.WriteLine("  Capabilities: " + service.GetCapabilities());

                        using (XtDevice defaultInput = service.OpenDefaultDevice(false)) {
                            Console.WriteLine("  Default input: " + defaultInput);
                        }
                        using (XtDevice defaultOutput = service.OpenDefaultDevice(true)) {
                            Console.WriteLine("  Default output: " + defaultOutput);
                        }

                        for (int d = 0; d < service.GetDeviceCount(); d++)
                            using (XtDevice device = service.OpenDevice(d)) {
                                Console.WriteLine("  Device " + device.GetName() + ":");
                                Console.WriteLine("    System: " + device.GetSystem());
                                Console.WriteLine("    Current mix: " + device.GetMix());
                                Console.WriteLine("    Input channels: " + device.GetChannelCount(false));
                                Console.WriteLine("    Output channels: " + device.GetChannelCount(true));
                                Console.WriteLine("    Interleaved access: " + device.SupportsAccess(true));
                                Console.WriteLine("    Non-interleaved access: " + device.SupportsAccess(false));
                            }
                    }
                }
            } catch (XtException e) {
                Console.WriteLine(string.Format("Error: system {0}, fault {1}, cause {2}, text {3}, message: {4}.\n",
                    XtException.GetSystem(e.GetError()),
                    XtException.GetFault(e.GetError()),
                    XtException.GetCause(e.GetError()),
                    XtException.GetText(e.GetError()),
                    e.ToString()));
            }
        }
    }
}
