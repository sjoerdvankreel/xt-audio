using System;
using System.Threading;

namespace Xt
{
    public class RenderSimple
    {
        static double phase = 0.0;
        const double Frequency = 660.0;
        static readonly XtFormat Format = new XtFormat(new XtMix(44100, XtSample.Float32), 0, 0, 1, 0);

        static void Render(XtStream stream, object input, object output, int frames,
                double time, ulong position, bool timeValid, ulong error, object user)
        {
            for (int f = 0; f < frames; f++)
            {
                phase += Frequency / Format.mix.rate;
                if (phase >= 1.0)
                    phase = -1.0;
                ((float[])output)[f] = (float)Math.Sin(phase * Math.PI);
            }
        }

        public static void Main(string[] args)
        {
            using (XtAudio audio = new XtAudio(null, IntPtr.Zero, null, null))
            {
                XtService service = XtAudio.GetServiceBySetup(XtSetup.ConsumerAudio);
                using (XtDevice device = service.OpenDefaultDevice(true))
                {
                    if (device != null && device.SupportsFormat(Format))
                    {

                        XtBuffer buffer = device.GetBuffer(Format);
                        using (XtStream stream = device.OpenStream(Format, true, false,
                                buffer.current, Render, null, null))
                        {
                            stream.Start();
                            Thread.Sleep(1000);
                            stream.Stop();
                        }
                    }
                }
            }
        }
    }
}