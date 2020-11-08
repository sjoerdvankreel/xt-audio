using System;

namespace Xt
{
    public class Aggregate
    {
        static void XRun(int index, IntPtr user)
        {
            // Don't do this.
            Console.WriteLine("XRun on device " + index + ".");
        }

        static void OnAggregate(IntPtr stream, in XtBuffer buffer, IntPtr user)
        {
            XtAdapter adapter = XtAdapter.Get(stream);
            XtFormat format = adapter.GetStream().GetFormat();
            XtAttributes attrs = XtAudio.GetSampleAttributes(format.mix.sample);
            adapter.LockBuffer(in buffer);
            if (buffer.frames > 0)
                Buffer.BlockCopy(adapter.GetInput(), 0, adapter.GetOutput(), 0, buffer.frames * format.channels.inputs * attrs.size);
            adapter.UnlockBuffer(in buffer);
        }

        public static void Main(string[] args)
        {
            XtMix mix = new XtMix(48000, XtSample.Int16);
            XtFormat inputFormat = new XtFormat(mix, new XtChannels(2, 0, 0, 0));
            XtFormat outputFormat = new XtFormat(mix, new XtChannels(0, 0, 2, 0));
            using (XtAudio audio = new XtAudio(null, IntPtr.Zero, null))
            {
                var system = XtAudio.SetupToSystem(XtSetup.SystemAudio);
                XtService service = XtAudio.GetService(system);
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
                            new XtChannels[] { inputFormat.channels, outputFormat.channels },
                            new double[] { 30.0, 30.0 },
                            2, mix, true, output, OnAggregate, XRun))
                    {
                        using XtAdapter adapter = XtAdapter.Register(stream, true, null);
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