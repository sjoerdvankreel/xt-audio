using System;

namespace Xt
{
	public static class PrintSimple
	{
		public static void Main()
		{
			using XtAudio audio = new XtAudio(null, IntPtr.Zero,  null);
			foreach(XtSystem system in XtAudio.GetSystems())
			{
				XtService service = XtAudio.GetService(system);
				for (int d = 0; d < service.GetDeviceCount(); d++)
					using (XtDevice device = service.OpenDevice(d))
						Console.WriteLine(system + ": " + device);
			}
		}
	}
}