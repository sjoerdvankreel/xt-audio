using System;

namespace Xt {

    public class RenderAdvanced {

        static double phase = 0.0;
        const double Frequency = 660.0;

        static void ReadLine() {
            Console.WriteLine("Press any key to continue...");
            Console.ReadLine();
        }

        static float NextSine(double sampleRate) {
            phase += Frequency / sampleRate;
            if (phase >= 1.0)
                phase = -1.0;
            return (float)Math.Sin(phase * Math.PI);
        }

        static void XRun(int index, object user) {
            // Don't do this.
            Console.WriteLine("XRun on device " + index + ", user = " + user + ".");
        }

        static void RenderInterleaved(XtStream stream, object input, object output,
            int frames, double time, ulong position, bool timeValid, ulong error, object user) {

            XtFormat format = stream.GetFormat();
            for (int f = 0; f < frames; f++) {
                float sine = NextSine(format.mix.rate);
                for (int c = 0; c < format.outputs; c++)
                    ((float[])output)[f * format.outputs + c] = sine;
            }
        }

        static unsafe void RenderInterleavedRaw(XtStream stream, object input, object output,
            int frames, double time, ulong position, bool timeValid, ulong error, object user) {

            XtFormat format = stream.GetFormat();
            for (int f = 0; f < frames; f++) {
                float sine = NextSine(format.mix.rate);
                for (int c = 0; c < format.outputs; c++)
                    ((float*)(IntPtr)output)[f * format.outputs + c] = sine;
            }
        }

        static void RenderNonInterleaved(XtStream stream, object input, object output,
            int frames, double time, ulong position, bool timeValid, ulong error, object user) {

            XtFormat format = stream.GetFormat();
            for (int f = 0; f < frames; f++) {
                float sine = NextSine(format.mix.rate);
                for (int c = 0; c < format.outputs; c++)
                    ((float[][])output)[c][f] = sine;
            }
        }

        static unsafe void RenderNonInterleavedRaw(XtStream stream, object input, object output,
            int frames, double time, ulong position, bool timeValid, ulong error, object user) {

            XtFormat format = stream.GetFormat();
            for (int f = 0; f < frames; f++) {
                float sine = NextSine(format.mix.rate);
                for (int c = 0; c < format.outputs; c++)
                    ((float**)(IntPtr)output)[c][f] = sine;
            }
        }

        public static void Main(String[] args) {

            using (XtAudio audio = new XtAudio(null, IntPtr.Zero, null, null)) {

                XtService service = XtAudio.GetServiceBySetup(XtSetup.ConsumerAudio);
                XtFormat format = new XtFormat(new XtMix(44100, XtSample.Float32), 0, 0, 2, 0);
                using (XtDevice device = service.OpenDefaultDevice(true)) {

                    if (device == null) {
                        Console.WriteLine("No default device found.");
                        return;
                    }

                    if (!device.SupportsFormat(format)) {
                        Console.WriteLine("Format not supported.");
                        return;
                    }

                    XtBuffer buffer = device.GetBuffer(format);

                    using (XtStream stream = device.OpenStream(format, true, false,
                        buffer.current, RenderInterleaved, XRun, "user-data")) {
                        stream.Start();
                        Console.WriteLine("Rendering interleaved...");
                        ReadLine();
                        stream.Stop();
                    }

                    using (XtStream stream = device.OpenStream(format, true, true,
                        buffer.current, RenderInterleavedRaw, XRun, "user-data")) {
                        stream.Start();
                        Console.WriteLine("Rendering interleaved, raw buffers...");
                        ReadLine();
                        stream.Stop();
                    }

                    using (XtStream stream = device.OpenStream(format, false, false,
                        buffer.current, RenderNonInterleaved, XRun, "user-data")) {
                        stream.Start();
                        Console.WriteLine("Rendering non-interleaved...");
                        ReadLine();
                        stream.Stop();
                    }

                    using (XtStream stream = device.OpenStream(format, false, true,
                        buffer.current, RenderNonInterleavedRaw, XRun, "user-data")) {
                        stream.Start();
                        Console.WriteLine("Rendering non-interleaved, raw buffers...");
                        ReadLine();
                        stream.Stop();
                    }

                    XtFormat sendTo0 = new XtFormat(new XtMix(44100, XtSample.Float32), 0, 0, 1, 1L << 0);
                    using (XtStream stream = device.OpenStream(sendTo0, true, false,
                        buffer.current, RenderInterleaved, XRun, "user-data")) {
                        stream.Start();
                        Console.WriteLine("Rendering channel mask, channel 0...");
                        ReadLine();
                        stream.Stop();
                    }

                    XtFormat sendTo1 = new XtFormat(new XtMix(44100, XtSample.Float32), 0, 0, 1, 1L << 1);
                    using (XtStream stream = device.OpenStream(sendTo1, true, false, buffer.current,
                            RenderInterleaved, XRun, "user-data")) {
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