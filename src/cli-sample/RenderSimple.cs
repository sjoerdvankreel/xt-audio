using System;
using System.Threading;

namespace Xt
{
	class State
	{
		public double phase = 0.0;
		public readonly double Frequency = 440.0;
		public readonly XtFormat Format = new XtFormat(new XtMix(44100, XtSample.Float32), new XtChannels(0, 0, 1, 0));
	}

	public class RenderSimple
	{
		static void Render(IntPtr s, in XtBuffer b, in XtTime time, ulong error, IntPtr u)
		{
			var adapter = XtAdapter.Get(s);
			var state = (State)adapter.User;
			using var io = adapter.IO();
			for (int f = 0; f < b.frames; f++)
			{
				state.phase += state.Frequency / state.Format.mix.rate;
				if (state.phase >= 1.0) state.phase = -1.0;
				((float[])adapter.Output)[f] = (float)Math.Sin(2.0 * state.phase * Math.PI);
			}
		}

		public static void Main(string[] args)
		{
			using XtAudio audio = new XtAudio(null, IntPtr.Zero, null);
			XtService service = XtAudio.GetService(XtAudio.SetupToSystem(XtSetup.ConsumerAudio));
			if (service == null) return;
			using XtDevice device = service.OpenDefaultDevice(true);
			if (device?.SupportsFormat(Format) != true) return;
			XtBufferSize size = device.GetBufferSize(Format);
			using XtStream stream = device.OpenStream(Format, true, size.current, Render, null);
			using XtAdapter adapter = XtAdapter.Register(stream, null);
			stream.Start();
			Thread.Sleep(1000);
			stream.Stop();
		}
	}
}