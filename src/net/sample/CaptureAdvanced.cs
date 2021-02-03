using System;
using System.IO;
using System.Runtime.InteropServices;
using System.Threading;

namespace Xt
{
    public class CaptureAdvanced
    {
        class Context
        {
            internal byte[] intermediate;
            internal FileStream recording;
        }

        static readonly XtMix Mix = new XtMix(44100, XtSample.Int24);
        static readonly XtChannels Channels = new XtChannels(2, 0, 0, 0);
        static readonly XtFormat Format = new XtFormat(Mix, Channels);

        // Normally don't do I/O in the callback.
        static void OnXRun(XtStream stream, int index, object user)
        => Console.WriteLine("XRun on device " + index + ".");

        static void OnRunning(XtStream stream, bool running, ulong error, object user)
        {
            string evt = running ? "Started" : "Stopped";
            Console.WriteLine("Stream event: " + evt + ", new state: " + stream.IsRunning() + ".");
            if (error != 0) Console.WriteLine(XtAudio.GetErrorInfo(error).ToString());
        }

        static void RunStream(XtStream stream)
        {
            stream.Start();
            Thread.Sleep(2000);
            stream.Stop();
        }

        static int GetBufferSize(int channels, int frames)
        {
            int size = XtAudio.GetSampleAttributes(Mix.sample).size;
            return channels * frames * size;
        }

        // Normally don't do I/O in the callback.
        static int OnInterleavedSafeBuffer(XtStream stream, in XtBuffer buffer, object user)
        {
            var output = (FileStream)user;
            XtSafeBuffer safe = XtSafeBuffer.Get(stream);
            int bytes = GetBufferSize(Channels.inputs, buffer.frames);
            safe.Lock(in buffer);
            output.Write((byte[])safe.GetInput(), 0, bytes);
            safe.Unlock(in buffer);
            return 0;
        }

        // Normally don't do I/O in the callback.
        static int OnInterleavedNativeBuffer(XtStream stream, in XtBuffer buffer, object user)
        {
            var ctx = (Context)user;
            int bytes = GetBufferSize(Channels.inputs, buffer.frames);
            Marshal.Copy(buffer.input, ctx.intermediate, 0, bytes);
            ctx.recording.Write(ctx.intermediate, 0, bytes);
            return 0;
        }

        // Normally don't do I/O in the callback.
        static int OnNonInterleavedSafeBuffer(XtStream stream, in XtBuffer buffer, object user)
        {
            var output = (FileStream)user;
            XtSafeBuffer safe = XtSafeBuffer.Get(stream);
            int size = XtAudio.GetSampleAttributes(Mix.sample).size;
            safe.Lock(in buffer);
            for (int f = 0; f < buffer.frames; f++)
                for (int c = 0; c < Channels.inputs; c++)
                    output.Write(((byte[][])safe.GetInput())[c], f * size, size);
            safe.Unlock(in buffer);
            return 0;
        }

        // Normally don't do I/O in the callback.
        static unsafe int OnNonInterleavedNativeBuffer(XtStream stream, in XtBuffer buffer, object user)
        {
            var ctx = (Context)user;
            int size = XtAudio.GetSampleAttributes(Mix.sample).size;
            for (int f = 0; f < buffer.frames; f++)
                for (int c = 0; c < Channels.inputs; c++)
                {
                    IntPtr source = new IntPtr(&(((byte**)buffer.input)[c][f * size]));
                    Marshal.Copy(source, ctx.intermediate, 0, size);
                    ctx.recording.Write(ctx.intermediate, 0, size);
                }
            return 0;
        }

        [STAThread]
        public static void Main()
        {
            XtStreamParams streamParams;
            XtDeviceStreamParams deviceParams;

            using XtPlatform platform = XtAudio.Init(null, IntPtr.Zero, null);
            XtSystem system = platform.SetupToSystem(XtSetup.ConsumerAudio);
            XtService service = platform.GetService(system);
            if (service == null) return;

            string defaultInput = service.GetDefaultDeviceId(false);
            if (defaultInput == null) return;
            using XtDevice device = service.OpenDevice(defaultInput);
            if (!device.SupportsFormat(Format)) return;
            XtBufferSize size = device.GetBufferSize(Format);

            Console.WriteLine("Capture interleaved, safe buffers...");
            streamParams = new XtStreamParams(true, OnInterleavedSafeBuffer, OnXRun, OnRunning);
            deviceParams = new XtDeviceStreamParams(in streamParams, in Format, size.current);
            using (FileStream recording = new FileStream("xt-audio-interleaved-safe.raw", FileMode.Create, FileAccess.Write))
            using (XtStream stream = device.OpenStream(in deviceParams, recording))
            using (XtSafeBuffer safe = XtSafeBuffer.Register(stream, true))
                RunStream(stream);

            Console.WriteLine("Capture interleaved, native buffers...");
            var context = new Context();
            streamParams = new XtStreamParams(true, OnInterleavedNativeBuffer, OnXRun, OnRunning);
            deviceParams = new XtDeviceStreamParams(in streamParams, in Format, size.current);
            using (FileStream recording = new FileStream("xt-audio-interleaved-native.raw", FileMode.Create, FileAccess.Write))
            using (XtStream stream = device.OpenStream(in deviceParams, context))
            {
                context.recording = recording;
                context.intermediate = new byte[GetBufferSize(Channels.inputs, stream.GetFrames())];
                RunStream(stream);
            }

            Console.WriteLine("Capture non-interleaved, safe buffers...");
            streamParams = new XtStreamParams(false, OnNonInterleavedSafeBuffer, OnXRun, OnRunning);
            deviceParams = new XtDeviceStreamParams(in streamParams, in Format, size.current);
            using (FileStream recording = new FileStream("xt-audio-non-interleaved-safe.raw", FileMode.Create, FileAccess.Write))
            using (XtStream stream = device.OpenStream(in deviceParams, recording))
            using (XtSafeBuffer safe = XtSafeBuffer.Register(stream, false))
                RunStream(stream);

            Console.WriteLine("Capture non-interleaved, native buffers...");
            context = new Context();
            streamParams = new XtStreamParams(false, OnNonInterleavedNativeBuffer, OnXRun, OnRunning);
            deviceParams = new XtDeviceStreamParams(in streamParams, in Format, size.current);
            using (FileStream recording = new FileStream("xt-audio-non-interleaved-native.raw", FileMode.Create, FileAccess.Write))
            using (XtStream stream = device.OpenStream(in deviceParams, context))
            {
                context.recording = recording;
                context.intermediate = new byte[GetBufferSize(Channels.inputs, stream.GetFrames())];
                RunStream(stream);
            }
        }
    }
}