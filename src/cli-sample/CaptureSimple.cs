using System;
using System.IO;
using System.Threading;

namespace Xt {

    public class CaptureSimple {

        const int SampleSize = 3;
        static readonly XtFormat Format = new XtFormat(new XtMix(44100, XtSample.Int24), 1, 0, 0, 0);

        static void Capture(XtStream stream, object input, object output, int frames,
            double time, ulong position, bool timeValid, ulong error, object user) {

            if (frames > 0)
                // Don't do this.
                ((FileStream)user).Write((byte[])input, 0, frames * SampleSize);
        }

        public static void Main(string[] args) {

            using (XtAudio audio = new XtAudio(null, IntPtr.Zero, null, null)) {
                XtService service = XtAudio.GetServiceBySetup(XtSetup.ConsumerAudio);
                using (XtDevice device = service.OpenDefaultDevice(false))
                    if (device != null && device.SupportsFormat(Format)) {

                        XtBuffer buffer = device.GetBuffer(Format);
                        using (FileStream recording = new FileStream(
                            "xt-audio.raw", FileMode.Create, FileAccess.Write))
                        using (XtStream stream = device.OpenStream(Format, true, false,
                            buffer.current, Capture, recording)) {
                            stream.Start();
                            Thread.Sleep(1000);
                            stream.Stop();
                        }
                    }
            }
        }
    }
}