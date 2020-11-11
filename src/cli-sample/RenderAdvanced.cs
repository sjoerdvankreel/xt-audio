using System;
using System.Threading;

namespace Xt
{
    public class RenderAdvanced
    {
        static float _phase = 0.0f;
        const float Frequency = 440.0f;
        static readonly XtMix Mix = new XtMix(44100, XtSample.Float32);

        static float NextSample()
        {
            _phase += Frequency / Mix.rate;
            if (_phase >= 1.0) _phase = -1.0f;
            return (float)Math.Sin(2.0 * _phase * Math.PI);
        }

        static void XRun(int index, object user)
        => Console.WriteLine("XRun on device " + index + ".");

        static void RunStream(XtStream stream)
        {
            stream.Start();
            Thread.Sleep(2000);
            stream.Stop();
        }

        static void RenderInterleavedSafe(XtStream stream, in XtBuffer buffer, object user)
        {
            XtSafeBuffer safe = XtSafeBuffer.Get(stream);
            int channels = stream.GetFormat().channels.outputs;
            safe.Lock(in buffer);
            float[] output = (float[])safe.GetOutput();
            for (int f = 0; f < buffer.frames; f++)
            {
                float sample = NextSample();
                for (int c = 0; c < channels; c++) output[f * channels + c] = sample;
            }
            safe.Unlock(buffer);
        }

        static unsafe void RenderInterleavedNative(XtStream stream, in XtBuffer buffer, object user)
        {
            int channels = stream.GetFormat().channels.outputs;
            int size = XtAudio.GetSampleAttributes(Mix.sample).size;
            for (int f = 0; f < buffer.frames; f++)
            {
                float sample = NextSample();
                for (int c = 0; c < channels; c++) ((float*)buffer.output)[f * channels + c] = sample;
            }
        }

        static void RenderNonInterleavedSafe(XtStream stream, in XtBuffer buffer, object user)
        {
            XtSafeBuffer safe = XtSafeBuffer.Get(stream);
            int channels = stream.GetFormat().channels.outputs;
            safe.Lock(buffer);
            float[][] output = (float[][])safe.GetOutput();
            for (int f = 0; f < buffer.frames; f++)
            {
                float sample = NextSample();
                for (int c = 0; c < channels; c++) output[c][f] = sample;
            }
            safe.Unlock(buffer);
        }

        static unsafe void RenderNonInterleavedNative(XtStream stream, in XtBuffer buffer, object user)
        {
            int channels = stream.GetFormat().channels.outputs;
            int size = XtAudio.GetSampleAttributes(Mix.sample).size;
            for (int f = 0; f < buffer.frames; f++)
            {
                float sample = NextSample();
                for (int c = 0; c < channels; c++) ((float**)buffer.output)[c][f] = sample;
            }
        }

        public static void Main()
        {
            using XtAudio audio = new XtAudio(null, IntPtr.Zero, null);
            XtSystem system = XtAudio.SetupToSystem(XtSetup.ConsumerAudio);
            XtService service = XtAudio.GetService(system);
            if (service == null) return;
            XtFormat format = new XtFormat(Mix, new XtChannels(0, 0, 2, 0));
            using XtDevice device = service.OpenDefaultDevice(true);
            if (device?.SupportsFormat(format) != true) return;
            XtBufferSize size = device.GetBufferSize(format);

            Console.WriteLine("Render interleaved, safe buffers...");
            using (XtStream stream = device.OpenStream(format, true, size.current, RenderInterleavedSafe, XRun, null))
            using (XtSafeBuffer safe = XtSafeBuffer.Register(stream, true))
                RunStream(stream);

            Console.WriteLine("Render interleaved, native buffers...");
            using (XtStream stream = device.OpenStream(format, true, size.current, RenderInterleavedNative, XRun, null))
                RunStream(stream);

            Console.WriteLine("Render non-interleaved, safe buffers...");
            using (XtStream stream = device.OpenStream(format, false, size.current, RenderNonInterleavedSafe, XRun, null))
            using (XtSafeBuffer safe = XtSafeBuffer.Register(stream, false))
                RunStream(stream);

            Console.WriteLine("Render non-interleaved, native buffers...");
            using (XtStream stream = device.OpenStream(format, false, size.current, RenderNonInterleavedNative, XRun, null))
                RunStream(stream);

            Console.WriteLine("Render interleaved, safe buffers (channel 0)...");
            XtFormat sendTo0 = new XtFormat(Mix, new XtChannels(0, 0, 1, 1L << 0));
            using (XtStream stream = device.OpenStream(sendTo0, true, size.current, RenderInterleavedSafe, XRun, null))
            using (XtSafeBuffer safe = XtSafeBuffer.Register(stream, true))
                RunStream(stream);

            Console.WriteLine("Render interleaved, native buffers (channel 1)...");
            XtFormat sendTo1 = new XtFormat(Mix, new XtChannels(0, 0, 1, 1L << 1));
            using (XtStream stream = device.OpenStream(sendTo1, true, size.current, RenderInterleavedNative, XRun, null))
                RunStream(stream);
        }
    }
}