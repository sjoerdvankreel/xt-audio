using System;
using System.Threading;

namespace Xt
{
    public class FullDuplex
    {
        static void Callback(XtStream stream, in XtBuffer buffer, object user)
        {
            XtSafeBuffer safe = XtSafeBuffer.Get(stream);
            safe.Lock(in buffer);
            Buffer.BlockCopy(safe.GetInput(), 0, safe.GetOutput(), 0, buffer.frames * 2 * 4);
            safe.Unlock(in buffer);
        }

        public static void Main()
        {
            XtFormat format;
            XtStreamParams streamParams;
            XtDeviceStreamParams deviceParams;
            XtFormat int44100 = new XtFormat(new XtMix(44100, XtSample.Int32), new XtChannels(2, 0, 2, 0));
            XtFormat int48000 = new XtFormat(new XtMix(48000, XtSample.Int32), new XtChannels(2, 0, 2, 0));
            XtFormat float44100 = new XtFormat(new XtMix(44100, XtSample.Float32), new XtChannels(2, 0, 2, 0));
            XtFormat float48000 = new XtFormat(new XtMix(48000, XtSample.Float32), new XtChannels(2, 0, 2, 0));

            using XtAudio audio = new XtAudio(null, IntPtr.Zero, null);
            XtSystem system = XtAudio.SetupToSystem(XtSetup.ProAudio);
            XtService service = XtAudio.GetService(system);
            if (service == null) return;

            using XtDevice device = service.OpenDefaultDevice(true);
            if (device == null) return;
            if (device.SupportsFormat(int44100)) format = int44100;
            else if (device.SupportsFormat(int48000)) format = int48000;
            else if (device.SupportsFormat(float44100)) format = float44100;
            else if (device.SupportsFormat(float48000)) format = float48000;
            else return;

            XtBufferSize size = device.GetBufferSize(format);
            streamParams = new XtStreamParams(true, Callback, null);
            deviceParams = new XtDeviceStreamParams(in streamParams, in format, size.current);
            using XtStream stream = device.OpenStream(in deviceParams, null);
            using XtSafeBuffer safe = XtSafeBuffer.Register(stream, true);
            stream.Start();
            Thread.Sleep(2000);
            stream.Stop();
        }
    }
}