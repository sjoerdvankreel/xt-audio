using System;
using System.IO;
using System.Threading;

namespace Xt {

    public static class CaptureDefaultInput {

        const int Channels = 2;
        const XtSample Sample = XtSample.Int24;

        internal static void Capture(XtStream stream, Array input, Array output, int frames,
                double time, ulong position, bool timeValid, ulong error, object user) {

            int bufferSize = frames * Channels * XtAudio.GetSampleAttributes(Sample).size;
            if (frames == 0)
                return;
            // Don't do this.
            ((FileStream)user).Write(((byte[])input), 0, bufferSize);
        }

        [STAThread]
        public static void Main(string[] args) {

            using (XtAudio audio = new XtAudio(null, IntPtr.Zero, null, null)) {
                XtService service = XtAudio.GetServiceBySetup(XtSetup.ConsumerAudio);
                using (XtDevice device = service.OpenDefaultDevice(false)) {

                    if (device == null) {
                        Console.WriteLine("No default device found.");
                        return;
                    }

                    XtFormat format = new XtFormat(new XtMix(44100, Sample), Channels, 0, 0, 0);
                    if (!device.SupportsFormat(format)) {
                        Console.WriteLine("Format not supported.");
                        return;
                    }

                    XtBuffer buffer = device.GetBuffer(format);
                    using (FileStream recording = new FileStream("xt-audio.raw", FileMode.Create, FileAccess.Write))
                    using (XtStream stream = device.OpenStream(format, buffer.current, new XtStreamCallback(Capture), recording)) {
                        stream.Start();
                        Thread.Sleep(1000);
                        stream.Stop();
                    }
                }
            }
        }
    }
}