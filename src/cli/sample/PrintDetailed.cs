using System;

namespace Xt
{
    public static class PrintDetailed
    {
        static void OnError(string location, string message)
        => Console.WriteLine($"{location}: {message}");

        public static void Main()
        {
            using XtPlatform platform = XtAudio.Init("Sample", IntPtr.Zero, OnError);
            try
            {
                XtVersion version = XtAudio.GetVersion();
                Console.WriteLine("Version: " + version.major + "." + version.minor);
                XtSystem pro = XtAudio.SetupToSystem(XtSetup.ProAudio);
                Console.WriteLine("Pro Audio: " + pro + " (" + (platform.GetService(pro) != null) + ")");
                XtSystem system = XtAudio.SetupToSystem(XtSetup.SystemAudio);
                Console.WriteLine("System Audio: " + system + " (" + (platform.GetService(system) != null) + ")");
                XtSystem consumer = XtAudio.SetupToSystem(XtSetup.ConsumerAudio);
                Console.WriteLine("Consumer Audio: " + consumer + " (" + (platform.GetService(consumer) != null) + ")");

                foreach (XtSystem s in platform.GetSystems())
                {
                    XtService service = platform.GetService(s);
                    Console.WriteLine("System: " + s);
                    Console.WriteLine("  Device count: " + service.GetDeviceCount());
                    Console.WriteLine("  Capabilities: " + service.GetCapabilities());
                    using XtDevice defaultInput = service.OpenDefaultDevice(false);
                    if (defaultInput != null) Console.WriteLine("  Default input: " + defaultInput);
                    using XtDevice defaultOutput = service.OpenDefaultDevice(true);
                    if (defaultOutput != null) Console.WriteLine("  Default output: " + defaultOutput);

                    for (int d = 0; d < service.GetDeviceCount(); d++)
                        using (XtDevice device = service.OpenDevice(d))
                        {
                            try
                            {
                                XtMix? mix = device.GetMix();
                                Console.WriteLine("  Device " + device + ":");
                                Console.WriteLine("    Input channels: " + device.GetChannelCount(false));
                                Console.WriteLine("    Output channels: " + device.GetChannelCount(true));
                                Console.WriteLine("    Interleaved access: " + device.SupportsAccess(true));
                                Console.WriteLine("    Non-interleaved access: " + device.SupportsAccess(false));
                                if (mix != null) Console.WriteLine("    Current mix: " + mix.Value.rate + " " + mix.Value.sample);
                            } catch (XtException e)
                            { Console.WriteLine(XtAudio.GetErrorInfo(e.GetError())); }
                        }
                }
            } catch (XtException e)
            { Console.WriteLine(XtAudio.GetErrorInfo(e.GetError())); }
        }
    }
}