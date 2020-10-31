using System;

namespace Xt
{
    public class FullDuplex
    {
        static void Callback(XtStream stream, object input, object output,
            int frames, double time, ulong position, bool timeValid, ulong error, object user)
        {
            Buffer.BlockCopy((Array)input, 0, (Array)output, 0, frames * 2 * 4);
        }

        public static void Main(string[] args)
        {
            XtFormat format;
            XtFormat int44100 = new XtFormat(new XtMix(44100, XtSample.Int32), new XtChannels(2, 0, 2, 0));
            XtFormat int48000 = new XtFormat(new XtMix(48000, XtSample.Int32), new XtChannels(2, 0, 2, 0));
            XtFormat float44100 = new XtFormat(new XtMix(44100, XtSample.Float32), new XtChannels(2, 0, 2, 0));
            XtFormat float48000 = new XtFormat(new XtMix(48000, XtSample.Float32), new XtChannels(2, 0, 2, 0));

            using (XtAudio audio = new XtAudio(null, IntPtr.Zero, null, null))
            {
                var system = XtAudio.SetupToSystem(XtSetup.ProAudio);
                XtService service = XtAudio.GetService(system);
                if (service == null)
                    return;

                using (XtDevice device = service.OpenDefaultDevice(true))
                {
                    if (device == null)
                        return;

                    if (device.SupportsFormat(int44100))
                        format = int44100;
                    else if (device.SupportsFormat(int48000))
                        format = int48000;
                    else if (device.SupportsFormat(float44100))
                        format = float44100;
                    else if (device.SupportsFormat(float48000))
                        format = float48000;
                    else
                        return;

                    XtBuffer buffer = device.GetBuffer(format);
                    using (XtStream stream = device.OpenStream(format, true,
                        false, buffer.min, Callback, null, null))
                    {
                        stream.Start();
                        Console.WriteLine("Streaming full-duplex, press any key to continue...");
                        Console.ReadLine();
                        stream.Stop();
                    }
                }
            }
        }
    }
}