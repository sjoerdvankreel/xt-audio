using System;

namespace Xt
{
    public class RenderAdvanced
    {
        static double phase = 0.0;
        const double Frequency = 440.0;

        static void ReadLine()
        {
            Console.WriteLine("Press any key to continue...");
            Console.ReadLine();
        }

        static float NextSine(double sampleRate)
        {
            phase += Frequency / sampleRate;
            if (phase >= 1.0)
                phase = -1.0;
            return (float)Math.Sin(2.0 * phase * Math.PI);
        }

        static void XRun(int index, IntPtr user)
        {
            // Don't do this.
            Console.WriteLine("XRun on device " + index + ".");
        }

        static void RenderInterleavedManaged(IntPtr stream, in XtBuffer buffer, IntPtr user)
        {
            XtAdapter adapter = XtAdapter.Get(stream);
            XtFormat format = adapter.GetStream().GetFormat();
            adapter.LockBuffer(in buffer);
            for (int f = 0; f < buffer.frames; f++)
            {
                float sine = NextSine(format.mix.rate);
                for (int c = 0; c < format.channels.outputs; c++)
                    ((float[])adapter.GetOutput())[f * format.channels.outputs + c] = sine;
            }
            adapter.UnlockBuffer(in buffer);
        }

        static unsafe void RenderInterleavedNative(IntPtr stream, in XtBuffer buffer, IntPtr user)
        {
            XtAdapter adapter = XtAdapter.Get(stream);
            XtFormat format = adapter.GetStream().GetFormat();
            for (int f = 0; f < buffer.frames; f++)
            {
                float sine = NextSine(format.mix.rate);
                for (int c = 0; c < format.channels.outputs; c++)
                    ((float*)buffer.output)[f * format.channels.outputs + c] = sine;
            }
        }

        static void RenderNonInterleavedManaged(IntPtr stream, in XtBuffer buffer, IntPtr user)
        {
            XtAdapter adapter = XtAdapter.Get(stream);
            XtFormat format = adapter.GetStream().GetFormat();
            adapter.LockBuffer(in buffer);
            for (int f = 0; f < buffer.frames; f++)
            {
                float sine = NextSine(format.mix.rate);
                for (int c = 0; c < format.channels.outputs; c++)
                    ((float[][])adapter.GetOutput())[c][f] = sine;
            }
            adapter.UnlockBuffer(in buffer);
        }

        static unsafe void RenderNonInterleavedNative(IntPtr stream, in XtBuffer buffer, IntPtr user)
        {
            XtAdapter adapter = XtAdapter.Get(stream);
            XtFormat format = adapter.GetStream().GetFormat();
            for (int f = 0; f < buffer.frames; f++)
            {
                float sine = NextSine(format.mix.rate);
                for (int c = 0; c < format.channels.outputs; c++)
                    ((float**)buffer.output)[c][f] = sine;
            }
        }

        public static void Main()
        {
            using (XtAudio audio = new XtAudio(null, IntPtr.Zero,  null))
            {
                var system = XtAudio.SetupToSystem(XtSetup.ConsumerAudio);
                XtService service = XtAudio.GetService(system);
                if (service == null)
                    return;

                XtFormat format = new XtFormat(new XtMix(44100, XtSample.Float32), new XtChannels(0, 0, 2, 0));
                using (XtDevice device = service.OpenDefaultDevice(true))
                {
                    if (device == null || !device.SupportsFormat(format))
                        return;

                    XtBufferSize size = device.GetBufferSize(format);
                    using (XtStream stream = device.OpenStream(format, true, 
                        size.current, RenderInterleavedManaged, XRun))
                    {
                        using var adapter = XtAdapter.Register(stream, true, null);
                        stream.Start();
                        Console.WriteLine("Rendering interleaved...");
                        ReadLine();
                        stream.Stop();
                    }

                    using (XtStream stream = device.OpenStream(format, true, 
                        size.current, RenderInterleavedNative, XRun))
                    {
                        using var adapter = XtAdapter.Register(stream, true, null);
                        stream.Start();
                        Console.WriteLine("Rendering interleaved, raw buffers...");
                        ReadLine();
                        stream.Stop();
                    }

                    using (XtStream stream = device.OpenStream(format, false, 
                        size.current, RenderNonInterleavedManaged, XRun))
                    {
                        using var adapter = XtAdapter.Register(stream, false, null);
                        stream.Start();
                        Console.WriteLine("Rendering non-interleaved...");
                        ReadLine();
                        stream.Stop();
                    }

                    using (XtStream stream = device.OpenStream(format, false, 
                        size.current, RenderNonInterleavedNative, XRun))
                    {
                        using var adapter = XtAdapter.Register(stream, false, null);
                        stream.Start();
                        Console.WriteLine("Rendering non-interleaved, raw buffers...");
                        ReadLine();
                        stream.Stop();
                    }

                    XtFormat sendTo0 = new XtFormat(new XtMix(44100, XtSample.Float32), new XtChannels(0, 0, 1, 1L << 0));
                    using (XtStream stream = device.OpenStream(sendTo0, true, 
                        size.current, RenderInterleavedManaged, XRun))
                    {
                        using var adapter = XtAdapter.Register(stream, true, null);
                        stream.Start();
                        Console.WriteLine("Rendering channel mask, channel 0...");
                        ReadLine();
                        stream.Stop();
                    }

                    XtFormat sendTo1 = new XtFormat(new XtMix(44100, XtSample.Float32), new XtChannels(0, 0, 1, 1L << 1));
                    using (XtStream stream = device.OpenStream(sendTo1, true,  size.current,
                            RenderInterleavedManaged, XRun))
                    {
                        using var adapter = XtAdapter.Register(stream, true, null);
                        stream.Start();
                        Console.WriteLine("Rendering channel mask, channel 1...");
                        ReadLine();
                        stream.Stop();
                    }
                }
            }
        }
    }
}