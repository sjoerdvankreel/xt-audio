using System;
using System.Threading;

namespace Xt {

    public static class RenderDefaultOutput {

        static readonly Random Rand = new Random();

        static void Render(XtStream stream, object input, object output, int frames,
                double time, ulong position, bool timeValid, ulong error, Object user) {

            short[] buffer = (short[])output;
            XtFormat format = stream.GetFormat();
            for (int f = 0; f < frames; f++)
                for (int c = 0; c < format.outputs; c++) {
                    double noise = Rand.NextDouble() * 2.0 - 1.0;
                    buffer[f * format.outputs + c] = (short)(noise * short.MaxValue);
                }
        }

        [STAThread]
        public static void Main(string[] args) {

            using (XtAudio audio = new XtAudio(null, IntPtr.Zero, null, null)) {
                XtService service = XtAudio.GetServiceBySetup(XtSetup.ConsumerAudio);
                using (XtDevice device = service.OpenDefaultDevice(true)) {

                    if (device == null) {
                        Console.WriteLine("No default device found.");
                        return;
                    }

                    XtFormat format = new XtFormat(new XtMix(44100, XtSample.Int16), 0, 0, 2, 0);
                    if (!device.SupportsFormat(format)) {
                        Console.WriteLine("Format not supported.");
                        return;
                    }

                    XtBuffer buffer = device.GetBuffer(format);
                    using (XtStream stream = device.OpenStream(format, true, false, buffer.current, Render, "user-data")) {
                        stream.Start();
                        Thread.Sleep(1000);
                        stream.Stop();
                    }
                }
            }
        }
    }
}
