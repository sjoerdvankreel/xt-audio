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

		static void OnBuffer(IntPtr stream, in XtBuffer buffer, in XtTime time, ulong error, IntPtr _)
		{
			var adapter = XtAdapter.Get(stream);
			adapter.LockBuffer(buffer);
			float[] output = (float[])adapter.Output;
			for (int f = 0; f < buffer.frames; f++) output[f] = NextSample();
			adapter.UnlockBuffer(buffer);
		}

		public static void Main()
		{
			using XtAudio audio = new XtAudio(null, IntPtr.Zero, null);
			var system = XtAudio.SetupToSystem(XtSetup.ConsumerAudio);
			XtService service = XtAudio.GetService(system);
			if (service == null) return;
			using XtDevice device = service.OpenDefaultDevice(true);
			if (device?.SupportsFormat(Format) != true) return;
			XtBufferSize size = device.GetBufferSize(Format);
			using XtStream stream = device.OpenStream(Format, true, size.current, OnBuffer, null);
			using XtAdapter adapter = XtAdapter.Register(stream, null);
			stream.Start();
			Thread.Sleep(1000);
			stream.Stop();
		}
	}
}