using System;
using System.IO;
using System.Threading;

namespace Xt
{
	public class CaptureSimple
	{
		const int SampleSize = 3;
		static readonly XtFormat Format = new XtFormat(new XtMix(44100, XtSample.Int24), new XtChannels(1, 0, 0, 0));

		static void Capture(IntPtr stream, in XtBuffer buffer, IntPtr user)
		{
			var adapter = XtAdapter.Get(stream);
			adapter.LockBuffer(in buffer);
			// Don't do this.
			if (buffer.frames > 0)
				((FileStream)adapter.GetUser()).Write((byte[])adapter.GetInput(), 0, buffer.frames * SampleSize);
			adapter.UnlockBuffer(in buffer);
		}

		public static void Main(string[] args)
		{
			using (XtAudio audio = new XtAudio(null, IntPtr.Zero, null))
			{
				var system = XtAudio.SetupToSystem(XtSetup.ConsumerAudio);
				XtService service = XtAudio.GetService(system);
				if (service == null)
					return;

				using (XtDevice device = service.OpenDefaultDevice(false))
				{
					if (device == null || !device.SupportsFormat(Format))
						return;

					XtBufferSize size = device.GetBufferSize(Format);
					using (FileStream recording = new FileStream(
						"xt-audio.raw", FileMode.Create, FileAccess.Write))
					using (XtStream stream = device.OpenStream(Format, true,
						size.current, Capture, null))
					{
						using var adapter = XtAdapter.Register(stream, true, recording);
						stream.Start();
						Thread.Sleep(1000);
						stream.Stop();
					}
				}
			}
		}
	}
}