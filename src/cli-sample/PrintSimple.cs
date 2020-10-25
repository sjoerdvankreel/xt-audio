using System;

namespace Xt
{
	public static class PrintSimple
	{
		public static void Main()
		{
			using var audio = new XtAudio(null, IntPtr.Zero, null, null);
			for (int s = 0; s < XtAudio.GetServiceCount(); s++)
			{
				var service = XtAudio.GetServiceByIndex(s);
				for (int d = 0; d < service.GetDeviceCount(); d++)
					using (var device = service.OpenDevice(d))
						Console.WriteLine(service + ": " + device);
			}
		}
	}
}