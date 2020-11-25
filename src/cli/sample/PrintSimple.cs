using System;

namespace Xt
{
	public static class PrintSimple
	{
		public static void Main()
		{
			using XtPlatform platform = XtAudio.Init(null, IntPtr.Zero,  null);
			foreach(XtSystem system in platform.GetSystems())
			{
				XtService service = platform.GetService(system);
				for (int d = 0; d < service.GetDeviceCount(); d++)
					using (XtDevice device = service.OpenDevice(d))
						Console.WriteLine(system + ": " + device);
			}
		}
	}
}