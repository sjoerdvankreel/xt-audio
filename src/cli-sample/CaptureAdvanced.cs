using System;
using System.IO;
using System.Runtime.InteropServices;

namespace Xt {

    public class CaptureAdvanced {

        class Context {

            internal byte[] intermediate;
            internal FileStream recording;
        }

        static void ReadLine() {
            Console.WriteLine("Press any key to continue...");
            Console.ReadLine();
        }

        static int GetBufferSize(XtStream stream, int frames) {
            XtFormat format = stream.GetFormat();
            int sampleSize = XtAudio.GetSampleAttributes(format.mix.sample).size;
            return frames * format.inputs * sampleSize;
        }

        static void CaptureInterleaved(XtStream stream, object input, object output,
            int frames, double time, ulong position, bool timeValid, ulong error, object user) {

            if (frames > 0)
                // Don't do this.
                ((Context)user).recording.Write((byte[])input, 0, GetBufferSize(stream, frames));
        }

        static void CaptureInterleavedRaw(XtStream stream, object input, object output,
            int frames, double time, ulong position, bool timeValid, ulong error, object user) {

            if (frames > 0) {
                Context ctx = (Context)user;
                Marshal.Copy((IntPtr)input, ctx.intermediate, 0, GetBufferSize(stream, frames));
                // Don't do this.
                ctx.recording.Write(ctx.intermediate, 0, GetBufferSize(stream, frames));
            }
        }

        static void CaptureNonInterleaved(XtStream stream, object input, object output,
            int frames, double time, ulong position, bool timeValid, ulong error, object user) {

            if (frames > 0) {
                Context ctx = (Context)user;
                XtFormat format = stream.GetFormat();
                int channels = format.inputs;
                int sampleSize = XtAudio.GetSampleAttributes(format.mix.sample).size;
                for (int f = 0; f < frames; f++)
                    for (int c = 0; c < channels; c++)
                        // Don't do this.
                        ctx.recording.Write(((byte[][])input)[c], f * sampleSize, sampleSize);
            }
        }

        static unsafe void CaptureNonInterleavedRaw(XtStream stream, object input, object output,
            int frames, double time, ulong position, bool timeValid, ulong error, object user) {

            if (frames > 0) {
                Context ctx = (Context)user;
                XtFormat format = stream.GetFormat();
                int channels = format.inputs;
                int sampleSize = XtAudio.GetSampleAttributes(format.mix.sample).size;
                for (int f = 0; f < frames; f++)
                    for (int c = 0; c < channels; c++) {
                        IntPtr source = new IntPtr(&(((byte**)(IntPtr)input)[c][f * sampleSize]));
                        Marshal.Copy(source, ctx.intermediate, 0, sampleSize);
                        // Don't do this.
                        ctx.recording.Write(ctx.intermediate, 0, sampleSize);
                    }
            }
        }

        public static void Main(string[] args) {

            using (XtAudio audio = new XtAudio(null, IntPtr.Zero, null, null)) {

                XtService service = XtAudio.GetServiceBySetup(XtSetup.ConsumerAudio);
                XtFormat format = new XtFormat(new XtMix(44100, XtSample.Int24), 2, 0, 0, 0);
                using (XtDevice device = service.OpenDefaultDevice(false)) {

                    if (device == null) {
                        Console.WriteLine("No default device found.");
                        return;
                    }

                    if (!device.SupportsFormat(format)) {
                        Console.WriteLine("Format not supported.");
                        return;
                    }

                    Context context = new Context();
                    XtBuffer buffer = device.GetBuffer(format);

                    using (FileStream recording = new FileStream(
                        "xt-audio-interleaved.raw", FileMode.Create, FileAccess.Write))
                    using (XtStream stream = device.OpenStream(format, true, false,
                        buffer.current, CaptureInterleaved, null, context)) {

                        context.recording = recording;
                        context.intermediate = new byte[GetBufferSize(stream, stream.GetFrames())];
                        stream.Start();
                        Console.WriteLine("Capturing interleaved...");
                        ReadLine();
                        stream.Stop();
                    }

                    using (FileStream recording = new FileStream(
                        "xt-audio-interleaved-raw.raw", FileMode.Create, FileAccess.Write))
                    using (XtStream stream = device.OpenStream(format, true, true,
                        buffer.current, CaptureInterleavedRaw, null, context)) {

                        context.recording = recording;
                        context.intermediate = new byte[GetBufferSize(stream, stream.GetFrames())];
                        stream.Start();
                        Console.WriteLine("Capturing interleaved, raw buffers...");
                        ReadLine();
                        stream.Stop();
                    }

                    using (FileStream recording = new FileStream(
                        "xt-audio-non-interleaved.raw", FileMode.Create, FileAccess.Write))
                    using (XtStream stream = device.OpenStream(format, false, false,
                        buffer.current, CaptureNonInterleaved, null, context)) {

                        context.recording = recording;
                        context.intermediate = new byte[GetBufferSize(stream, stream.GetFrames())];
                        stream.Start();
                        Console.WriteLine("Capturing non-interleaved...");
                        ReadLine();
                        stream.Stop();
                    }

                    using (FileStream recording = new FileStream(
                        "xt-audio-non-interleaved-raw.raw", FileMode.Create, FileAccess.Write))
                    using (XtStream stream = device.OpenStream(format, false, true,
                        buffer.current, CaptureNonInterleavedRaw, null, context)) {

                        context.recording = recording;
                        context.intermediate = new byte[GetBufferSize(stream, stream.GetFrames())];
                        stream.Start();
                        Console.WriteLine("Capturing non-interleaved, raw buffers...");
                        ReadLine();
                        stream.Stop();
                    }
                }
            }
        }
    }
}