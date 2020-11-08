using System;
using System.IO;
using System.Runtime.InteropServices;

namespace Xt
{
    public class CaptureAdvanced
    {
        class Context
        {
            internal byte[] intermediate;
            internal FileStream recording;
        }

        static void ReadLine()
        {
            Console.WriteLine("Press any key to continue...");
            Console.ReadLine();
        }

        static int GetBufferSize(XtStream stream, int frames)
        {
            XtFormat format = stream.GetFormat();
            int sampleSize = XtAudio.GetSampleAttributes(format.mix.sample).size;
            return frames * format.channels.inputs * sampleSize;
        }

        static void XRun(int index, IntPtr user)
        {
            // Don't do this.
            Console.WriteLine("XRun on device " + index + ".");
        }

        static void CaptureInterleavedManaged(IntPtr stream, in XtBuffer buffer, IntPtr user)
        {
            XtAdapter adapter = XtAdapter.Get(stream);
            adapter.LockBuffer(in buffer);
            // Don't do this.
            if (buffer.frames > 0)
                ((Context)adapter.GetUser()).recording.Write((byte[])adapter.GetInput(), 0, GetBufferSize(adapter.GetStream(), buffer.frames));
            adapter.UnlockBuffer(in buffer);
        }

        static void CaptureInterleavedNative(IntPtr stream, in XtBuffer buffer, IntPtr user)
        {
            XtAdapter adapter = XtAdapter.Get(stream);
            if (buffer.frames > 0)
            {
                Context ctx = (Context)adapter.GetUser();
                Marshal.Copy(buffer.input, ctx.intermediate, 0, GetBufferSize(adapter.GetStream(), buffer.frames));
                // Don't do this.
                ctx.recording.Write(ctx.intermediate, 0, GetBufferSize(adapter.GetStream(), buffer.frames));
            }
        }

        static void CaptureNonInterleavedManaged(IntPtr stream, in XtBuffer buffer, IntPtr user)
        {
            XtAdapter adapter = XtAdapter.Get(stream);
            adapter.LockBuffer(in buffer);
            if (buffer.frames > 0)
            {
                Context ctx = (Context)adapter.GetUser();
                XtFormat format = adapter.GetStream().GetFormat();
                int channels = format.channels.inputs;
                int sampleSize = XtAudio.GetSampleAttributes(format.mix.sample).size;
                for (int f = 0; f < buffer.frames; f++)
                    for (int c = 0; c < channels; c++)
                        // Don't do this.
                        ctx.recording.Write(((byte[][])adapter.GetInput())[c], f * sampleSize, sampleSize);
            }
            adapter.UnlockBuffer(in buffer);
        }

        static unsafe void CaptureNonInterleavedNative(IntPtr stream, in XtBuffer buffer, IntPtr user)
        {
            XtAdapter adapter = XtAdapter.Get(stream);
            if (buffer.frames > 0)
            {
                Context ctx = (Context)adapter.GetUser();
                XtFormat format = adapter.GetStream().GetFormat();
                int channels = format.channels.inputs;
                int sampleSize = XtAudio.GetSampleAttributes(format.mix.sample).size;
                for (int f = 0; f < buffer.frames; f++)
                    for (int c = 0; c < channels; c++)
                    {
                        IntPtr source = new IntPtr(&(((byte**)(IntPtr)buffer.input)[c][f * sampleSize]));
                        Marshal.Copy(source, ctx.intermediate, 0, sampleSize);
                        // Don't do this.
                        ctx.recording.Write(ctx.intermediate, 0, sampleSize);
                    }
            }
        }

        public static void Main(string[] args)
        {
            using (XtAudio audio = new XtAudio(null, IntPtr.Zero, null))
            {
                var system = XtAudio.SetupToSystem(XtSetup.ConsumerAudio);
                XtService service = XtAudio.GetService(system);
                if (service == null)
                    return;

                XtFormat format = new XtFormat(new XtMix(44100, XtSample.Int24), new XtChannels(2, 0, 0, 0));
                using (XtDevice device = service.OpenDefaultDevice(false))
                {
                    if (device == null || !device.SupportsFormat(format))
                        return;

                    Context context = new Context();
                    XtBufferSize size = device.GetBufferSize(format);

                    using (FileStream recording = new FileStream(
                        "xt-audio-interleaved.raw", FileMode.Create, FileAccess.Write))
                    using (XtStream stream = device.OpenStream(format, true, 
                        size.current, CaptureInterleavedManaged, XRun))
                    {
                        using var adapter = XtAdapter.Register(stream, true, context);
                        context.recording = recording;
                        context.intermediate = new byte[GetBufferSize(stream, stream.GetFrames())];
                        stream.Start();
                        Console.WriteLine("Capturing interleaved...");
                        ReadLine();
                        stream.Stop();
                    }

                    using (FileStream recording = new FileStream(
                        "xt-audio-interleaved-raw.raw", FileMode.Create, FileAccess.Write))
                    using (XtStream stream = device.OpenStream(format, true, 
                        size.current, CaptureInterleavedNative, XRun))
                    {
                        using var adapter = XtAdapter.Register(stream, true, context);
                        context.recording = recording;
                        context.intermediate = new byte[GetBufferSize(stream, stream.GetFrames())];
                        stream.Start();
                        Console.WriteLine("Capturing interleaved, raw buffers...");
                        ReadLine();
                        stream.Stop();
                    }

                    using (FileStream recording = new FileStream(
                        "xt-audio-non-interleaved.raw", FileMode.Create, FileAccess.Write))
                    using (XtStream stream = device.OpenStream(format, false, 
                        size.current, CaptureNonInterleavedManaged, XRun))
                    {
                        using var adapter = XtAdapter.Register(stream, false, context);
                        context.recording = recording;
                        context.intermediate = new byte[GetBufferSize(stream, stream.GetFrames())];
                        stream.Start();
                        Console.WriteLine("Capturing non-interleaved...");
                        ReadLine();
                        stream.Stop();
                    }

                    using (FileStream recording = new FileStream(
                        "xt-audio-non-interleaved-raw.raw", FileMode.Create, FileAccess.Write))
                    using (XtStream stream = device.OpenStream(format, false, 
                        size.current, CaptureNonInterleavedNative, XRun))
                    {
                        using var adapter = XtAdapter.Register(stream, false, context);
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