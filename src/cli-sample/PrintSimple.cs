using System;

namespace Xt
{
	public static class PrintSimple
	{
		public static void Main()
		{
			using var audio = new XtAudio(null, IntPtr.Zero, null, null);
			foreach(var s in XtAudio.GetSystems())
			{
				var service = XtAudio.GetService(s);
				for (int d = 0; d < service.GetDeviceCount(); d++)
					using (var device = service.OpenDevice(d))
						Console.WriteLine(service + ": " + device);
			}
		}
	}
}