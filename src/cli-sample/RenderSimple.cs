using System;
using System.Threading;

namespace Xt
{
    public class RenderSimple
    {
        static float _phase;
        const float Frequency = 440.0f;
        static readonly XtMix Mix = new XtMix(44100, XtSample.Float32);
        static readonly XtChannels Channels = new XtChannels(0, 0, 1, 0);
        static readonly XtFormat Format = new XtFormat(Mix, Channels);

        static float NextSample()
        {
            _phase += Frequency / Mix.rate;
            if (_phase >= 1.0f) _phase = -1.0f;
            return (float)Math.Sin(2.0 * _phase * Math.PI);
        }

        static void Callback(XtStream stream, in XtBuffer buffer, object user)
        {
            XtSafeBuffer safe = XtSafeBuffer.Get(stream);
            safe.Lock(in buffer);
            float[] output = (float[])safe.GetOutput();
            for (int f = 0; f < buffer.frames; f++) output[f] = NextSample();
            safe.Unlock(in buffer);
        }

        public static void Main()
        {
            using XtAudio audio = new XtAudio(null, IntPtr.Zero, null);
            XtSystem system = XtAudio.SetupToSystem(XtSetup.ConsumerAudio);
            XtService service = XtAudio.GetService(system);
            if (service == null) return;
            using XtDevice device = service.OpenDefaultDevice(true);
            if (device?.SupportsFormat(Format) != true) return;
            XtBufferSize size = device.GetBufferSize(Format);
            using XtStream stream = device.OpenStream(Format, true, size.current, Callback, null, null);
            using XtSafeBuffer safe = XtSafeBuffer.Register(stream, true);
            stream.Start();
            Thread.Sleep(2000);
            stream.Stop();
        }
    }
}