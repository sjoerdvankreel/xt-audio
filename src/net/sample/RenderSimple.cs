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

        static int OnBuffer(XtStream stream, in XtBuffer buffer, object user)
        {
            XtSafeBuffer safe = XtSafeBuffer.Get(stream);
            safe.Lock(in buffer);
            float[] output = (float[])safe.GetOutput();
            for (int f = 0; f < buffer.frames; f++) output[f] = NextSample();
            safe.Unlock(in buffer);
            return 0;
        }

        [STAThread]
        public static void Main()
        {
            XtStreamParams streamParams;
            XtDeviceStreamParams deviceParams;

            using XtPlatform platform = XtAudio.Init(null, IntPtr.Zero);
            XtSystem system = platform.SetupToSystem(XtSetup.ConsumerAudio);
            XtService service = platform.GetService(system);
            if (service == null) return;

            string defaultOutput = service.GetDefaultDeviceId(true);
            if(defaultOutput == null) return;
            using XtDevice device = service.OpenDevice(defaultOutput);
            if (!device.SupportsFormat(Format)) return;

            XtBufferSize size = device.GetBufferSize(Format);
            streamParams = new XtStreamParams(true, OnBuffer, null, null);
            deviceParams = new XtDeviceStreamParams(in streamParams, in Format, size.current);
            using XtStream stream = device.OpenStream(in deviceParams, null);
            using XtSafeBuffer safe = XtSafeBuffer.Register(stream);
            stream.Start();
            Thread.Sleep(2000);
            stream.Stop();
        }
    }
}