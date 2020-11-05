using System;
using System.IO;
using System.Threading;

namespace Xt
{
	public class CaptureSimple
	{
		const int SampleSize = 3;
		static readonly XtFormat Format = new XtFormat(new XtMix(44100, XtSample.Int24), new XtChannels(1, 0, 0, 0));

		static void Capture(XtStream stream, in XtManagedBuffer buffer, in XtTime time, ulong error, object user)
		{
			// Don't do this.
			if (buffer.frames > 0)
				((FileStream)user).Write((byte[])buffer.input, 0, buffer.frames * SampleSize);
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
					using (XtStream stream = device.OpenStream(Format, true, size.current, Capture, null, recording))
					{
						stream.Start();
						Thread.Sleep(1000);
						stream.Stop();
					}
				}
			}
		}
	}
}