using System;

namespace Xt
{
    public class Aggregate
    {
        static void ReadLine()
        {
            Console.WriteLine("Press any key to continue...");
            Console.ReadLine();
        }

        static void XRun(int index, object user)
        {
            // Don't do this.
            Console.WriteLine("XRun on device " + index + ", user = " + user + ".");
        }

        static void OnAggregate(XtStream stream, object input, object output, int frames, double time,
                ulong position, bool timeValid, ulong error, object user)
        {
            XtFormat format = stream.GetFormat();
            XtAttributes attrs = XtAudio.GetSampleAttributes(format.mix.sample);
            if (frames > 0)
                Buffer.BlockCopy((Array)input, 0, (Array)output, 0, frames * format.inputs * attrs.size);
        }

        public static void Main(string[] args)
        {
            XtMix mix = new XtMix(48000, XtSample.Int16);
            XtFormat inputFormat = new XtFormat(mix, 2, 0, 0, 0);
            XtChannels inputChannels = new XtChannels(2, 0, 0, 0);
            XtFormat outputFormat = new XtFormat(mix, 0, 0, 2, 0);
            XtChannels outputChannels = new XtChannels(0, 0, 2, 0);
            using (XtAudio audio = new XtAudio(null, IntPtr.Zero, null, null))
            {
                XtService service = XtAudio.GetServiceBySetup(XtSetup.SystemAudio);
                if (service == null)
                    return;

                using (XtDevice input = service.OpenDefaultDevice(false))
                using (XtDevice output = service.OpenDefaultDevice(true))
                {
                    if (input == null || !input.SupportsFormat(inputFormat))
                        return;
                    if (output == null || !output.SupportsFormat(outputFormat))
                        return;

                    using (XtStream stream = service.AggregateStream(
                            new XtDevice[] { input, output },
                            new XtChannels[] { inputChannels, outputChannels },
                            new double[] { 30.0, 30.0 },
                            2, mix, true, false, output, OnAggregate, XRun, "user-data"))
                    {
                        stream.Start();
                        Console.WriteLine("Streaming aggregate, press any key to continue...");
                        Console.ReadLine();
                        stream.Stop();
                    }
                }
            }
        }
    }
}